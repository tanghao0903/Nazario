from dataclasses import dataclass
from datetime import datetime, timezone
from typing import Dict


@dataclass
class Quote:
    symbol: str
    last: float
    bid: float
    ask: float
    volume: int
    timestamp_utc: str


class MarketDataClient:
    def __init__(self, api_key: str, base_url: str) -> None:
        self.api_key = api_key
        self.base_url = base_url

    def get_quote(self, symbol: str) -> Quote:
        """Placeholder adapter. Replace with real market data API integration."""
        ts = datetime.now(timezone.utc).isoformat()
        return Quote(symbol=symbol, last=0.0, bid=0.0, ask=0.0, volume=0, timestamp_utc=ts)

    def get_quotes(self, symbols: list[str]) -> Dict[str, Quote]:
        return {symbol: self.get_quote(symbol) for symbol in symbols}
