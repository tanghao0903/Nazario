# Nazario Quant Architecture (Phase 1)

This repository contains a Phase 1 architecture for a local, model-assisted paper-trading workflow.

## Goal
Build a closed loop that combines:
- Grok real-time search signal input
- NASDAQ/NYSE real-time market data input
- Strategy decision engine in Codex
- Paper trading ledger with initial capital `10000 USD`
- Excel trade log output for audit and iteration

## Repository Layout
- `docs/quant-architecture.md`: Detailed architecture and execution flow
- `src/connectors/grok_client.py`: Grok search adapter interface
- `src/connectors/market_data_client.py`: Market data adapter interface
- `src/strategy/signal_engine.py`: Rule-based first-pass decision engine
- `src/simulator/paper_broker.py`: Portfolio and PnL simulation core
- `src/reporting/excel_logger.py`: Trade log export to `.xlsx`
- `src/main.py`: Example wiring for one analysis-execution cycle

## Phase 1 Scope
- Paper trading only, no broker order execution
- API integration via environment variables
- Deterministic trade ledger and repeatable simulation

## Quick Start
1. Copy `config/example.env` to `.env` and fill credentials.
2. Install dependencies: `pip install -r requirements.txt`.
3. Run: `python -m src.main`.
4. Inspect output in `output/trade_log.xlsx`.
