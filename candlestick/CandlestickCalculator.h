#pragma once
#include <vector>
#include <string>
#include "../OrderBookEntry.h"
#include "Candlestick.h"

enum CandlestickGranularity {
    Daily,
    Monthly,
    Yearly
};

class CandlestickCalculator {
public:
    static std::vector<Candlestick> buildCandlesticks(std::vector<OrderBookEntry>& entries,
                                               const std::string& product,
                                               const OrderBookType& type,
                                               const CandlestickGranularity& granularity);

private:
    static std::string getDateKey(const std::string& timestamp, const CandlestickGranularity& granularity);
};
