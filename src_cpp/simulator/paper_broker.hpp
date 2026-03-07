#pragma once

#include "common/models.hpp"

#include <string>
#include <unordered_map>

class PaperBroker {
public:
    PaperBroker(double initial_capital, double fee_rate, double max_position_notional);
    Trade execute(const std::string& symbol, const std::string& side, int quantity, double price, const std::string& reason, bool& ok);

    double cash() const { return cash_; }
    double realized_pnl() const { return realized_pnl_; }

private:
    std::string now_utc() const;

    double cash_;
    double fee_rate_;
    double max_position_notional_;
    std::unordered_map<std::string, int> positions_;
    std::unordered_map<std::string, double> avg_cost_;
    double realized_pnl_ {0.0};
};
