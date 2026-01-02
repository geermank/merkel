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
    /**
     * returns a string representing a simplification of the given timestamp, used to group many
     * different dates into bigger categories, based on the granularity. Examples:
     * input: 2026/01/01 22:32:51 --> output: 2026 (granularity yearly)
     * input: 2026/01/01 22:32:51 --> output: 2026/01 (granularity monthly)
     * input: 2026/01/01 22:32:51 --> output: 2026/01/01 (granularity daily)
     */
    static std::string getDateKey(const std::string& timestamp, const CandlestickGranularity& granularity);
};
