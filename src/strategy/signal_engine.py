from dataclasses import dataclass


@dataclass
class Decision:
    symbol: str
    action: str
    quantity: int
    reason: str
    confidence: float


class SignalEngine:
    def __init__(self, buy_threshold: float = 0.65, sell_threshold: float = -0.65) -> None:
        self.buy_threshold = buy_threshold
        self.sell_threshold = sell_threshold

    def decide(self, symbol: str, sentiment: float, confidence: float, price: float) -> Decision:
        if confidence < 0.5 or price <= 0:
            return Decision(symbol, "HOLD", 0, "Low confidence or invalid price", confidence)
        if sentiment >= self.buy_threshold:
            return Decision(symbol, "BUY", 1, "Positive signal exceeded threshold", confidence)
        if sentiment <= self.sell_threshold:
            return Decision(symbol, "SELL", 1, "Negative signal exceeded threshold", confidence)
        return Decision(symbol, "HOLD", 0, "No threshold hit", confidence)
