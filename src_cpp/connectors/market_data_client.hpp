#pragma once

#include "common/models.hpp"

#include <string>
#include <vector>

class MarketDataClient {
public:
    MarketDataClient(std::string api_key, std::string base_url, std::string quote_path);
    Quote get_quote(const std::string& symbol) const;

private:
    std::string api_key_;
    std::string base_url_;
    std::string quote_path_;
};
