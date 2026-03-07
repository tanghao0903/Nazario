#include "connectors/grok_client.hpp"

#include <algorithm>

GrokClient::GrokClient(std::string api_key, std::string base_url, std::string search_path)
    : api_key_(std::move(api_key)), base_url_(std::move(base_url)), search_path_(std::move(search_path)) {}

SearchSignal GrokClient::search_symbol(const std::string& symbol) const {
    SearchSignal signal;
    signal.symbol = symbol;

    if (base_url_.empty()) {
        signal.summary = "Missing GROK_BASE_URL";
        return signal;
    }

    // Placeholder for real HTTP integration. It keeps deterministic behavior for paper-trading bootstrap.
    // Replace with your Grok REST call and parse fields: sentiment [-1,1], confidence [0,1], summary text.
    const std::string upper = symbol;
    if (upper == "NVDA" || upper == "MSFT") {
        signal.sentiment = 0.72;
        signal.confidence = 0.68;
        signal.summary = "Mock positive event pulse";
    } else if (upper == "TSLA") {
        signal.sentiment = -0.71;
        signal.confidence = 0.66;
        signal.summary = "Mock negative event pulse";
    } else {
        signal.sentiment = 0.0;
        signal.confidence = 0.45;
        signal.summary = "Mock neutral event pulse";
    }

    signal.sentiment = std::max(-1.0, std::min(1.0, signal.sentiment));
    signal.confidence = std::max(0.0, std::min(1.0, signal.confidence));
    return signal;
}
