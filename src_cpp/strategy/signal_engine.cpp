#include "strategy/signal_engine.hpp"

#include <algorithm>

SignalEngine::SignalEngine(double buy_threshold, double sell_threshold, double min_confidence, double max_trade_notional)
    : buy_threshold_(buy_threshold),
      sell_threshold_(sell_threshold),
      min_confidence_(min_confidence),
      max_trade_notional_(max_trade_notional) {}

int SignalEngine::position_size(double price) const {
    if (price <= 0.0) {
        return 0;
    }
    const int qty = static_cast<int>(max_trade_notional_ / price);
    return std::max(1, qty);
}

Decision SignalEngine::decide(const std::string& symbol, double sentiment, double confidence, double price) const {
    Decision d;
    d.symbol = symbol;
    d.confidence = confidence;

    if (price <= 0.0 || confidence < min_confidence_) {
        d.action = "HOLD";
        d.reason = "Low confidence or invalid price";
        d.quantity = 0;
        return d;
    }

    d.quantity = position_size(price);

    if (sentiment >= buy_threshold_) {
        d.action = "BUY";
        d.reason = "Positive signal exceeded threshold";
        return d;
    }

    if (sentiment <= sell_threshold_) {
        d.action = "SELL";
        d.reason = "Negative signal exceeded threshold";
        return d;
    }

    d.action = "HOLD";
    d.reason = "No threshold hit";
    d.quantity = 0;
    return d;
}
