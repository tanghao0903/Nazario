# Quant System Architecture (Phase 1)

## 1. Product Objective
Create a local quantitative research and simulation stack that merges real-time search insights and exchange market data into executable paper-trading decisions.

## 2. Data Inputs
- Grok real-time search:
  - Targeted symbol query support
  - Event/news extraction
  - Optional sentiment score
- Market data API (NASDAQ/NYSE):
  - Real-time quote (bid, ask, last)
  - Volume and intraday metrics
  - Timestamped bars for strategy windows

## 3. Core Modules
- Ingestion layer:
  - `GrokClient` for search and event ingestion
  - `MarketDataClient` for quote/bar pull
- Decision layer:
  - `SignalEngine` computes buy/sell/hold action
  - Combines event confidence with market context
- Execution simulator:
  - `PaperBroker` enforces capital, position, and cash constraints
  - Tracks realized and unrealized PnL
- Reporting layer:
  - `ExcelLogger` writes trade-level and portfolio-level snapshots

## 4. Decision Contract
The strategy outputs a `Decision` object:
- symbol
- action (`BUY`, `SELL`, `HOLD`)
- quantity
- reason
- confidence

## 5. Risk Boundaries (Initial)
- Initial capital: `10000 USD`
- Max position size per symbol: configurable
- No shorting in Phase 1
- Hard stop-loss and optional take-profit
- Cooldown window after execution to avoid over-trading

## 6. Excel Output Contract
Minimum trade log columns:
- timestamp_utc
- symbol
- side
- quantity
- price
- notional
- fee
- realized_pnl
- cumulative_realized_pnl
- reason

Portfolio snapshot columns:
- timestamp_utc
- cash
- market_value
- equity
- unrealized_pnl
- realized_pnl

## 7. Runtime Flow
1. Pull latest market quotes for watchlist.
2. Pull latest Grok search signals for same symbols.
3. Build feature snapshot per symbol.
4. Run decision engine.
5. Simulate execution in paper broker.
6. Append rows to Excel and console summary.
7. Repeat by schedule.

## 8. Deployment Path
- Phase 1: local-only paper trading
- Phase 2: backtesting and parameter tuning
- Phase 3: optional real broker integration with explicit manual gate
