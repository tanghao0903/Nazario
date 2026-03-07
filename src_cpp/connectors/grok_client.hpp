#pragma once

#include "common/models.hpp"

#include <string>

class GrokClient {
public:
    GrokClient(std::string api_key, std::string base_url, std::string search_path);
    SearchSignal search_symbol(const std::string& symbol) const;

private:
    std::string api_key_;
    std::string base_url_;
    std::string search_path_;
};
