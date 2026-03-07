# Nazario Quant Architecture (Phase 1)

This repository now provides a C++ paper-trading scaffold for your Grok + NASDAQ/NYSE workflow.

## Goal
Build a closed loop that combines:
- Grok real-time search signal input
- NASDAQ/NYSE real-time market data input
- Strategy decision engine in Codex
- Paper trading ledger with initial capital `10000 USD`
- Trade log output (`.csv`, Excel-openable)

## Repository Layout
- `docs/quant-architecture.md`: Detailed architecture and execution flow
- `src_cpp/connectors/grok_client.*`: Grok adapter (currently deterministic placeholder)
- `src_cpp/connectors/market_data_client.*`: Market adapter (currently deterministic placeholder)
- `src_cpp/strategy/signal_engine.*`: Rule-based decision engine
- `src_cpp/simulator/paper_broker.*`: Cash/position/PnL simulator
- `src_cpp/reporting/trade_logger.*`: CSV trade logging
- `src_cpp/main.cpp`: End-to-end execution
- `CMakeLists.txt`: Build file

## Environment
Copy `config/example.env` to `.env` and set:
- `GROK_BASE_URL`, `GROK_API_KEY`, `GROK_SEARCH_PATH`
- `MARKET_DATA_BASE_URL`, `MARKET_DATA_API_KEY`, `MARKET_QUOTE_PATH`
- Risk controls: `MAX_POSITION_NOTIONAL`, `MAX_TRADE_NOTIONAL`, `MIN_CONFIDENCE`, thresholds

## Build & Run
1. Copy `config/example.env` to `.env`.
2. Configure: `cmake -S . -B build`
3. Build: `cmake --build build --config Release`
4. Run: `build/nazario_cpp`
5. Inspect `output/trade_log.csv` in Excel.

## Notes
- Current connectors are mock-safe placeholders so the flow runs deterministically.
- Next step is replacing the placeholder sections with your exact Grok and market API schemas.
