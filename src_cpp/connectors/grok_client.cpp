#include "connectors/grok_client.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace {
std::string trim_response(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    std::replace(s.begin(), s.end(), '\n', ' ');
    if (s.size() > 280) {
        s.resize(280);
    }
    return s;
}

std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 16);
    for (const char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string extract_json_string_field(const std::string& json, const std::string& field) {
    const std::string key = "\"" + field + "\"";
    const size_t key_pos = json.find(key);
    if (key_pos == std::string::npos) {
        return "";
    }

    const size_t colon_pos = json.find(':', key_pos + key.size());
    if (colon_pos == std::string::npos) {
        return "";
    }

    const size_t first_quote = json.find('"', colon_pos + 1);
    if (first_quote == std::string::npos) {
        return "";
    }

    std::string value;
    bool escaped = false;
    for (size_t i = first_quote + 1; i < json.size(); ++i) {
        const char c = json[i];
        if (escaped) {
            switch (c) {
                case 'n': value += '\n'; break;
                case 'r': value += '\r'; break;
                case 't': value += '\t'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                default: value += c; break;
            }
            escaped = false;
            continue;
        }

        if (c == '\\') {
            escaped = true;
            continue;
        }
        if (c == '"') {
            return value;
        }
        value += c;
    }

    return "";
}

double estimate_sentiment(const std::string& text) {
    std::string lowered = text;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    const std::array<const char*, 8> positive_words {
        "beat", "upgrade", "growth", "strong", "surge", "record", "bullish", "outperform"
    };
    const std::array<const char*, 8> negative_words {
        "miss", "downgrade", "weak", "lawsuit", "drop", "fraud", "bearish", "underperform"
    };

    int pos = 0;
    int neg = 0;
    for (const char* word : positive_words) {
        if (lowered.find(word) != std::string::npos) {
            ++pos;
        }
    }
    for (const char* word : negative_words) {
        if (lowered.find(word) != std::string::npos) {
            ++neg;
        }
    }

    const int total = pos + neg;
    if (total == 0) {
        return 0.0;
    }
    return static_cast<double>(pos - neg) / static_cast<double>(total);
}

std::string run_command_capture(const std::string& command) {
#ifdef _WIN32
    FILE* pipe = _popen(command.c_str(), "r");
#else
    FILE* pipe = popen(command.c_str(), "r");
#endif
    if (!pipe) {
        return "";
    }

    char buffer[4096];
    std::string output;
    while (fgets(buffer, static_cast<int>(sizeof(buffer)), pipe) != nullptr) {
        output += buffer;
    }

#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return output;
}
}

GrokClient::GrokClient(
    std::string api_key,
    std::string base_url,
    std::string search_path,
    std::string model,
    int timeout_sec
)
    : api_key_(std::move(api_key)),
      base_url_(std::move(base_url)),
      search_path_(std::move(search_path)),
      model_(std::move(model)),
      timeout_sec_(timeout_sec > 0 ? timeout_sec : 20) {}

SearchSignal GrokClient::search_symbol(const std::string& symbol) const {
    SearchSignal signal;
    signal.symbol = symbol;

    if (api_key_.empty()) {
        signal.summary = "Missing GROK_API_KEY";
        return signal;
    }
    if (base_url_.empty()) {
        signal.summary = "Missing GROK_BASE_URL";
        return signal;
    }

    std::string normalized_base = base_url_;
    while (!normalized_base.empty() && normalized_base.back() == '/') {
        normalized_base.pop_back();
    }

    std::string normalized_path = search_path_;
    if (normalized_path.empty()) {
        normalized_path = "/chat/completions";
    }
    if (normalized_path.front() != '/') {
        normalized_path = "/" + normalized_path;
    }

    const std::string url = normalized_base + normalized_path;
    const std::string model = model_.empty() ? "grok-3-latest" : model_;

    const std::string prompt =
        "You are a market signal assistant. Analyze current internet and X signals for ticker " + symbol +
        ". Return JSON only with keys: sentiment (number between -1 and 1), confidence (0 to 1), summary (short text).";

    const std::string payload =
        std::string("{") +
        "\"model\":\"" + json_escape(model) + "\"," +
        "\"messages\":[{" +
            "\"role\":\"system\",\"content\":\"You return strict JSON only.\"},{" +
            "\"role\":\"user\",\"content\":\"" + json_escape(prompt) + "\"}]," +
        "\"temperature\":0.1," +
        "\"tools\":[{" +
            "\"type\":\"web_search\"},{" +
            "\"type\":\"x_search\"}]}";

    const std::filesystem::path temp_path =
        std::filesystem::temp_directory_path() / ("grok_payload_" + symbol + ".json");

    {
        std::ofstream out(temp_path, std::ios::binary);
        out << payload;
    }

    const std::string command =
        "curl.exe -sS --max-time " + std::to_string(timeout_sec_) +
        " -X POST \"" + url + "\"" +
        " -H \"Content-Type: application/json\"" +
        " -H \"Authorization: Bearer " + api_key_ + "\"" +
        " --data-binary \"@" + temp_path.string() + "\"";

    const std::string raw = run_command_capture(command);
    std::error_code ec;
    std::filesystem::remove(temp_path, ec);

    if (raw.empty()) {
        signal.summary = "Grok request returned empty response";
        return signal;
    }

    const std::string content = extract_json_string_field(raw, "content");
    const std::string summary_from_json = extract_json_string_field(content, "summary");
    const std::string sentiment_text = extract_json_string_field(content, "sentiment");
    const std::string confidence_text = extract_json_string_field(content, "confidence");

    double sentiment = 0.0;
    double confidence = 0.45;

    try {
        if (!sentiment_text.empty()) {
            sentiment = std::stod(sentiment_text);
        } else {
            sentiment = estimate_sentiment(content);
        }
    } catch (...) {
        sentiment = estimate_sentiment(content);
    }

    try {
        if (!confidence_text.empty()) {
            confidence = std::stod(confidence_text);
        }
    } catch (...) {
        confidence = 0.55;
    }

    signal.sentiment = std::max(-1.0, std::min(1.0, sentiment));
    signal.confidence = std::max(0.0, std::min(1.0, confidence));
    signal.summary = trim_response(summary_from_json.empty() ? content : summary_from_json);

    if (signal.summary.empty()) {
        signal.summary = trim_response(raw);
    }

    return signal;
}
