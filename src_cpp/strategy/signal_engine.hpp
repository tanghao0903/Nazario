#pragma once

#include "common/models.hpp"

class SignalEngine {
public:
    SignalEngine(double buy_threshold, double sell_threshold, double min_confidence, double max_trade_notional);
    Decision decide(const std::string& symbol, double sentiment, double confidence, double price) const;

private:
    int position_size(double price) const;

    double buy_threshold_;
    double sell_threshold_;
    double min_confidence_;
    double max_trade_notional_;
};
