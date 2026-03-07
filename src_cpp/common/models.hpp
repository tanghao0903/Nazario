#pragma once

#include <string>

struct SearchSignal {
    std::string symbol;
    double sentiment {0.0};
    double confidence {0.0};
    std::string summary;
};

struct Quote {
    std::string symbol;
    double last {0.0};
    double bid {0.0};
    double ask {0.0};
    long long volume {0};
    std::string timestamp_utc;
};

struct Decision {
    std::string symbol;
    std::string action;
    int quantity {0};
    std::string reason;
    double confidence {0.0};
};

struct Trade {
    std::string timestamp_utc;
    std::string symbol;
    std::string side;
    int quantity {0};
    double price {0.0};
    double notional {0.0};
    double fee {0.0};
    double realized_pnl {0.0};
    std::string reason;
};
