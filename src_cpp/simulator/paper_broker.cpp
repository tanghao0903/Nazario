#include "simulator/paper_broker.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

PaperBroker::PaperBroker(double initial_capital, double fee_rate, double max_position_notional)
    : cash_(initial_capital), fee_rate_(fee_rate), max_position_notional_(max_position_notional) {}

std::string PaperBroker::now_utc() const {
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

Trade PaperBroker::execute(const std::string& symbol, const std::string& side, int quantity, double price, const std::string& reason, bool& ok) {
    ok = false;
    Trade t;
    t.timestamp_utc = now_utc();
    t.symbol = symbol;
    t.side = side;
    t.quantity = quantity;
    t.price = price;
    t.notional = quantity * price;
    t.fee = t.notional * fee_rate_;
    t.reason = reason;

    if (quantity <= 0 || price <= 0.0) {
        t.reason = "Rejected: invalid quantity or price";
        return t;
    }

    if (side == "BUY") {
        const double current_notional = static_cast<double>(positions_[symbol]) * price;
        if (current_notional + t.notional > max_position_notional_) {
            t.reason = "Rejected: max position notional exceeded";
            return t;
        }

        const double total_cost = t.notional + t.fee;
        if (total_cost > cash_) {
            t.reason = "Rejected: insufficient cash";
            return t;
        }

        const int prev_qty = positions_[symbol];
        const double prev_avg = avg_cost_[symbol];
        const int new_qty = prev_qty + quantity;
        const double new_avg = ((prev_qty * prev_avg) + t.notional) / static_cast<double>(new_qty);
        positions_[symbol] = new_qty;
        avg_cost_[symbol] = new_avg;
        cash_ -= total_cost;
        ok = true;
        return t;
    }

    if (side == "SELL") {
        const int prev_qty = positions_[symbol];
        if (quantity > prev_qty) {
            t.reason = "Rejected: insufficient position";
            return t;
        }

        const double avg = avg_cost_[symbol];
        t.realized_pnl = (price - avg) * quantity - t.fee;
        realized_pnl_ += t.realized_pnl;
        positions_[symbol] = prev_qty - quantity;
        if (positions_[symbol] == 0) {
            avg_cost_.erase(symbol);
        }
        cash_ += t.notional - t.fee;
        ok = true;
        return t;
    }

    t.reason = "Rejected: unsupported side";
    return t;
}
