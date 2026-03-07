#include "common/env.hpp"
#include "connectors/grok_client.hpp"
#include "connectors/market_data_client.hpp"
#include "reporting/trade_logger.hpp"
#include "simulator/paper_broker.hpp"
#include "strategy/signal_engine.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::vector<std::string> parse_watchlist(const std::string& raw) {
    std::vector<std::string> symbols;
    std::stringstream ss(raw);
    std::string item;
    while (std::getline(ss, item, ',')) {
        item.erase(std::remove_if(item.begin(), item.end(), ::isspace), item.end());
        if (!item.empty()) {
            symbols.push_back(item);
        }
    }
    return symbols;
}
}

int main() {
    Env env;
    env.load(".env");

    const std::string grok_api_key = env.get("GROK_API_KEY", "");
    const std::string grok_base_url = env.get("GROK_BASE_URL", "");
    const std::string grok_search_path = env.get("GROK_SEARCH_PATH", "/search");

    const std::string market_api_key = env.get("MARKET_DATA_API_KEY", "");
    const std::string market_base_url = env.get("MARKET_DATA_BASE_URL", "");
    const std::string market_quote_path = env.get("MARKET_QUOTE_PATH", "/quote");

    const double initial_capital = env.get_double("INITIAL_CAPITAL", 10000.0);
    const double max_position_notional = env.get_double("MAX_POSITION_NOTIONAL", 2500.0);
    const double max_trade_notional = env.get_double("MAX_TRADE_NOTIONAL", 1000.0);
    const double fee_rate = env.get_double("FEE_RATE", 0.0);

    const double buy_threshold = env.get_double("BUY_THRESHOLD", 0.65);
    const double sell_threshold = env.get_double("SELL_THRESHOLD", -0.65);
    const double min_confidence = env.get_double("MIN_CONFIDENCE", 0.5);

    const std::string watchlist_raw = env.get("WATCHLIST", "AAPL,MSFT,NVDA");
    const std::string log_path = env.get("TRADE_LOG_PATH", "output/trade_log.csv");

    const std::vector<std::string> watchlist = parse_watchlist(watchlist_raw);

    GrokClient grok(grok_api_key, grok_base_url, grok_search_path);
    MarketDataClient market(market_api_key, market_base_url, market_quote_path);
    SignalEngine strategy(buy_threshold, sell_threshold, min_confidence, max_trade_notional);
    PaperBroker broker(initial_capital, fee_rate, max_position_notional);
    TradeLogger logger(log_path);

    for (const std::string& symbol : watchlist) {
        const Quote quote = market.get_quote(symbol);
        const SearchSignal signal = grok.search_symbol(symbol);

        const Decision decision = strategy.decide(
            symbol,
            signal.sentiment,
            signal.confidence,
            quote.last
        );

        if (decision.action == "BUY" || decision.action == "SELL") {
            bool ok = false;
            Trade trade = broker.execute(
                decision.symbol,
                decision.action,
                decision.quantity,
                quote.last,
                decision.reason + " | " + signal.summary,
                ok
            );
            if (ok) {
                logger.append(trade, broker.realized_pnl());
            }
        }
    }

    std::cout << "Cash: " << broker.cash() << " | Realized PnL: " << broker.realized_pnl() << '\n';
    return 0;
}
