#include "reporting/trade_logger.hpp"

#include <filesystem>
#include <fstream>

TradeLogger::TradeLogger(std::string file_path) : file_path_(std::move(file_path)) {
    if (std::filesystem::exists(file_path_)) {
        has_header_written_ = true;
    }
}

void TradeLogger::append(const Trade& trade, double cumulative_realized_pnl) {
    std::filesystem::path p(file_path_);
    if (!p.parent_path().empty()) {
        std::filesystem::create_directories(p.parent_path());
    }

    std::ofstream out(file_path_, std::ios::app);
    if (!out.is_open()) {
        return;
    }

    if (!has_header_written_) {
        out << "timestamp_utc,symbol,side,quantity,price,notional,fee,realized_pnl,cumulative_realized_pnl,reason\n";
        has_header_written_ = true;
    }

    out << trade.timestamp_utc << ','
        << trade.symbol << ','
        << trade.side << ','
        << trade.quantity << ','
        << trade.price << ','
        << trade.notional << ','
        << trade.fee << ','
        << trade.realized_pnl << ','
        << cumulative_realized_pnl << ','
        << '"' << trade.reason << '"'
        << '\n';
}
