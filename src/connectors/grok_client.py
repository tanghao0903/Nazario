from dataclasses import dataclass
from typing import List


@dataclass
class SearchSignal:
    symbol: str
    sentiment: float
    confidence: float
    summary: str


class GrokClient:
    def __init__(self, api_key: str, base_url: str) -> None:
        self.api_key = api_key
        self.base_url = base_url

    def search_symbol(self, symbol: str) -> List[SearchSignal]:
        """Placeholder adapter. Replace with real Grok API integration."""
        return [
            SearchSignal(
                symbol=symbol,
                sentiment=0.0,
                confidence=0.0,
                summary="No live signal. Integrate Grok endpoint.",
            )
        ]
