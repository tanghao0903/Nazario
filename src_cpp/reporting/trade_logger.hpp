#pragma once

#include "common/models.hpp"

#include <string>

class TradeLogger {
public:
    explicit TradeLogger(std::string file_path);
    void append(const Trade& trade, double cumulative_realized_pnl);

private:
    std::string file_path_;
    bool has_header_written_ {false};
};
