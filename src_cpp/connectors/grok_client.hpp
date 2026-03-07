#pragma once

#include "common/models.hpp"

#include <string>

class GrokClient {
public:
    GrokClient(
        std::string api_key,
        std::string base_url,
        std::string search_path,
        std::string model,
        int timeout_sec
    );

    SearchSignal search_symbol(const std::string& symbol) const;

private:
    std::string api_key_;
    std::string base_url_;
    std::string search_path_;
    std::string model_;
    int timeout_sec_ {20};
};
