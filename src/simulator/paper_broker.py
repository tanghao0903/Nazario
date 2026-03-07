from dataclasses import dataclass
from datetime import datetime, timezone


@dataclass
class Trade:
    timestamp_utc: str
    symbol: str
    side: str
    quantity: int
    price: float
    notional: float
    fee: float
    realized_pnl: float
    reason: str


class PaperBroker:
    def __init__(self, initial_capital: float = 10000.0, fee_rate: float = 0.0) -> None:
        self.cash = initial_capital
        self.fee_rate = fee_rate
        self.positions: dict[str, int] = {}
        self.avg_cost: dict[str, float] = {}
        self.realized_pnl = 0.0

    def execute(self, symbol: str, side: str, quantity: int, price: float, reason: str) -> Trade | None:
        if quantity <= 0 or price <= 0:
            return None

        ts = datetime.now(timezone.utc).isoformat()
        notional = quantity * price
        fee = notional * self.fee_rate
        realized = 0.0

        if side == "BUY":
            total_cost = notional + fee
            if total_cost > self.cash:
                return None
            prev_qty = self.positions.get(symbol, 0)
            prev_cost = self.avg_cost.get(symbol, 0.0)
            new_qty = prev_qty + quantity
            new_avg = ((prev_qty * prev_cost) + notional) / new_qty
            self.positions[symbol] = new_qty
            self.avg_cost[symbol] = new_avg
            self.cash -= total_cost

        elif side == "SELL":
            prev_qty = self.positions.get(symbol, 0)
            if quantity > prev_qty:
                return None
            avg = self.avg_cost.get(symbol, 0.0)
            realized = (price - avg) * quantity - fee
            self.realized_pnl += realized
            self.positions[symbol] = prev_qty - quantity
            if self.positions[symbol] == 0:
                self.avg_cost.pop(symbol, None)
            self.cash += notional - fee
        else:
            return None

        return Trade(
            timestamp_utc=ts,
            symbol=symbol,
            side=side,
            quantity=quantity,
            price=price,
            notional=notional,
            fee=fee,
            realized_pnl=realized,
            reason=reason,
        )
