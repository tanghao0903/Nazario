from pathlib import Path
import pandas as pd

from src.simulator.paper_broker import Trade


class ExcelLogger:
    def __init__(self, path: str) -> None:
        self.path = Path(path)
        self.path.parent.mkdir(parents=True, exist_ok=True)

    def append_trade(self, trade: Trade, cumulative_realized_pnl: float) -> None:
        row = {
            "timestamp_utc": trade.timestamp_utc,
            "symbol": trade.symbol,
            "side": trade.side,
            "quantity": trade.quantity,
            "price": trade.price,
            "notional": trade.notional,
            "fee": trade.fee,
            "realized_pnl": trade.realized_pnl,
            "cumulative_realized_pnl": cumulative_realized_pnl,
            "reason": trade.reason,
        }

        if self.path.exists():
            df = pd.read_excel(self.path)
            df = pd.concat([df, pd.DataFrame([row])], ignore_index=True)
        else:
            df = pd.DataFrame([row])

        df.to_excel(self.path, index=False)
