import os
from dotenv import load_dotenv

from src.connectors.grok_client import GrokClient
from src.connectors.market_data_client import MarketDataClient
from src.reporting.excel_logger import ExcelLogger
from src.simulator.paper_broker import PaperBroker
from src.strategy.signal_engine import SignalEngine


def parse_watchlist(raw: str) -> list[str]:
    return [s.strip().upper() for s in raw.split(',') if s.strip()]


def main() -> None:
    load_dotenv()

    grok = GrokClient(
        api_key=os.getenv("GROK_API_KEY", ""),
        base_url=os.getenv("GROK_BASE_URL", ""),
    )
    market = MarketDataClient(
        api_key=os.getenv("MARKET_DATA_API_KEY", ""),
        base_url=os.getenv("MARKET_DATA_BASE_URL", ""),
    )

    initial_capital = float(os.getenv("INITIAL_CAPITAL", "10000"))
    watchlist = parse_watchlist(os.getenv("WATCHLIST", "AAPL,MSFT,NVDA"))
    trade_log_path = os.getenv("TRADE_LOG_PATH", "output/trade_log.xlsx")

    engine = SignalEngine()
    broker = PaperBroker(initial_capital=initial_capital)
    logger = ExcelLogger(trade_log_path)

    quotes = market.get_quotes(watchlist)

    for symbol in watchlist:
        quote = quotes[symbol]
        signals = grok.search_symbol(symbol)
        signal = signals[0]
        decision = engine.decide(
            symbol=symbol,
            sentiment=signal.sentiment,
            confidence=signal.confidence,
            price=quote.last,
        )

        if decision.action in {"BUY", "SELL"}:
            trade = broker.execute(
                symbol=decision.symbol,
                side=decision.action,
                quantity=decision.quantity,
                price=quote.last,
                reason=decision.reason,
            )
            if trade is not None:
                logger.append_trade(trade, broker.realized_pnl)

    print(f"Cash: {broker.cash:.2f} | Realized PnL: {broker.realized_pnl:.2f}")


if __name__ == "__main__":
    main()
