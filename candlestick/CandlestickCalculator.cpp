#include "CandlestickCalculator.h"
#include <map>
#include <algorithm>
#include <iostream>

#include "../OrderBook.h"

std::string CandlestickCalculator::getDateKey(const std::string& timestamp, const CandlestickGranularity& granularity)
{
    if (timestamp.size() < 10) return timestamp;

    if (granularity == Daily) {
        return timestamp.substr(0, 10);
    }
    if (granularity == Monthly) {
        if (timestamp.size() >= 7) return timestamp.substr(0, 7);
        return timestamp;
    }
    if (granularity == Yearly) {
        if (timestamp.size() >= 4) return timestamp.substr(0, 4);
        return timestamp;
    }

    return timestamp.substr(0, 4);
}

std::vector<Candlestick> CandlestickCalculator::buildCandlesticks(std::vector<OrderBookEntry>& entries,
                                                                  const std::string& product,
                                                                  const OrderBookType& type,
                                                                  const CandlestickGranularity& granularity)
{
    std::vector<Candlestick> result;

    std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByTimestamp);

    // use a map to group candles with the same temporality
    std::map<std::string, std::vector<OrderBookEntry>> candlesData;
    for (const OrderBookEntry& e : entries) {
        std::string key = getDateKey(e.timestamp, granularity);
        candlesData[key].push_back(e);
    }

    for (std::pair<std::string, std::vector<OrderBookEntry>> candle : candlesData) {
        std::string candleKey = candle.first;
        std::vector<OrderBookEntry> candleEntries = candle.second;

        if (candleEntries.empty()) continue;

        double open = candleEntries[0].price;
        double close = candleEntries[candleEntries.size() - 1].price;

        double high = OrderBook::getHighPrice(candleEntries);
        double low =  OrderBook::getLowPrice(candleEntries);

        Candlestick c {product, OrderBookEntry::orderBookTypeToString(type), candleKey, open, high, low, close};
        result.push_back(c);
    }

    return result;
}
