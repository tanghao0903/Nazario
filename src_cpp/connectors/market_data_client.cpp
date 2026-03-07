#include "connectors/market_data_client.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace {
std::string now_utc() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm_utc {};
#ifdef _WIN32
    gmtime_s(&tm_utc, &t);
#else
    gmtime_r(&t, &tm_utc);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm_utc, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}
}

MarketDataClient::MarketDataClient(std::string api_key, std::string base_url, std::string quote_path)
    : api_key_(std::move(api_key)), base_url_(std::move(base_url)), quote_path_(std::move(quote_path)) {}

Quote MarketDataClient::get_quote(const std::string& symbol) const {
    Quote q;
    q.symbol = symbol;
    q.timestamp_utc = now_utc();

    if (base_url_.empty()) {
        return q;
    }

    // Placeholder for real HTTP integration. Replace with parsed fields from market API.
    if (symbol == "NVDA") {
        q.last = 895.0;
    } else if (symbol == "MSFT") {
        q.last = 415.0;
    } else if (symbol == "AAPL") {
        q.last = 195.0;
    } else if (symbol == "TSLA") {
        q.last = 185.0;
    } else {
        q.last = 100.0;
    }

    q.bid = q.last - 0.05;
    q.ask = q.last + 0.05;
    q.volume = 1000000;
    return q;
}
