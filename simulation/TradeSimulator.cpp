#include "TradeSimulator.h"
#include "../CSVReader.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

double TradeSimulator::randBetween(double low, double high) {
    // I got this logic from:
    // https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c
    double r = (double) std::rand() / (double) RAND_MAX;
    return low + r * (high - low);
}

void TradeSimulator::addBalanceToWalletIfNeeded(const std::vector<std::string>& products,
                                        Wallet& wallet,
                                        TransactionManager& txManager,
                                        const std::string& username)
{
    // If wallet is empty add small balances so we can always generate orders
    std::vector<std::string> currencies;

    for (const std::string& p : products) {
        std::vector<std::string> parts = CSVReader::tokenise(p, '/');
        if (parts.size() == 2) {
            currencies.push_back(parts[0]);
            currencies.push_back(parts[1]);
        }
    }

    std::sort(currencies.begin(), currencies.end());

    // get rid of duplicates
    std::map<std::string, bool> uniqueCurrencies;
    for (const std::string& c : currencies) {
        uniqueCurrencies[c] = true;
    }
    currencies.clear();
    for (const auto& p : uniqueCurrencies) {
        currencies.push_back(p.first);
    }


    bool hasAny = false;
    for (const std::string& c : currencies) {
        if (wallet.getWalletBalanceForCurrency(c) > 0) {
            hasAny = true;
            break;
        }
    }
    if (hasAny) return;

    for (const std::string& c : currencies) {
        wallet.insertCurrency(c, 10.0);
        Transaction t{
            username,
            txManager.nowTimestamp(),
            "DEPOSIT",
            c,
            c,
            10.0,
            wallet.getWalletBalanceForCurrency(c)
        };
        txManager.append(t);
    }
}

double TradeSimulator::computeMidPrice(OrderBook& orderBook,
                                       const std::string& product,
                                       const std::string& timestamp)
{
    auto asks = orderBook.getOrders(OrderBookType::ask, product, timestamp);
    auto bids = orderBook.getOrders(OrderBookType::bid, product, timestamp);

    if (asks.empty() || bids.empty()) {
        // fallback: if one side empty at that timestamp, look at global orders for product
        auto asksAll = orderBook.getOrders(OrderBookType::ask, product);
        auto bidsAll = orderBook.getOrders(OrderBookType::bid, product);
        if (asksAll.empty() || bidsAll.empty()) return 1.0;

        double bestAsk = OrderBook::getLowPrice(asksAll);
        double bestBid = OrderBook::getHighPrice(bidsAll);
        return (bestAsk + bestBid) / 2.0;
    }

    double bestAsk = OrderBook::getLowPrice(asks);
    double bestBid = OrderBook::getHighPrice(bids);
    return (bestAsk + bestBid) / 2.0;
}

void TradeSimulator::addMarketLiquidityAtTimestamp(OrderBook& orderBook,
                                                   const std::vector<std::string>& products,
                                                   const std::string& sourceTimestamp,
                                                   const std::string& newTimestamp)
{
    // Duplicate a small slice of the market depth at "sourceTimestamp" to "newTimestamp"
    // so orders with system timestamp can actually match.
    // This does NOT modify the market file; it’s in-memory only.
    for (const std::string& product : products) {
        auto asks = orderBook.getOrders(OrderBookType::ask, product, sourceTimestamp);
        auto bids = orderBook.getOrders(OrderBookType::bid, product, sourceTimestamp);

        // keep only a small depth (10 each) to avoid exploding the book
        std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
        std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

        int depth = 10;
        for (int i = 0; i < (int)asks.size() && i < depth; i++) {
            OrderBookEntry e{asks[i].price, asks[i].amount, newTimestamp, product, OrderBookType::ask, "dataset"};
            orderBook.insertOrder(e);
        }
        for (int i = 0; i < (int)bids.size() && i < depth; i++) {
            OrderBookEntry e{bids[i].price, bids[i].amount, newTimestamp, product, OrderBookType::bid, "dataset"};
            orderBook.insertOrder(e);
        }
    }
}

void TradeSimulator::createOrdersForProduct(OrderBook& orderBook,
                                            Wallet& wallet,
                                            TransactionManager& txManager,
                                            const std::string& username,
                                            const std::string& product,
                                            const std::string& nowTs,
                                            int n)
{
    auto parts = CSVReader::tokenise(product, '/');
    if (parts.size() != 2) return;
    std::string base = parts[0];
    std::string quote = parts[1];

    // Use mid price derived from LAST market timestamp (we pass that in by preparing liquidity)
    // Prices:
    // - ask slightly above mid
    // - bid slightly below mid
    double mid = computeMidPrice(orderBook, product, nowTs);

    for (int i = 0; i < n; i++) {
        // ASK
        {
            double baseBal = wallet.getWalletBalanceForCurrency(base);
            double maxAmt = baseBal * 0.20;                 // at most 20% of balance
            double amt = (maxAmt > 0) ? randBetween(maxAmt * 0.10, maxAmt) : 0.0;

            double askPrice = mid * (1.0 + randBetween(0.005, 0.02)); // +0.5%..+2%

            if (amt > 0) {
                OrderBookEntry ask{askPrice, amt, nowTs, product, OrderBookType::ask, username};
                if (wallet.canFulfillOrder(ask)) {
                    orderBook.insertOrder(ask);

                    Transaction t{
                        username,
                        txManager.nowTimestamp(),   // transaction log time
                        "ASK",
                        product,
                        base,
                        amt,
                        wallet.getWalletBalanceForCurrency(base)
                    };
                    txManager.append(t);
                }
            }
        }

        // BID
        {
            double quoteBal = wallet.getWalletBalanceForCurrency(quote);
            double bidPrice = mid * (1.0 - randBetween(0.005, 0.02)); // -0.5%..-2%

            double maxSpend = quoteBal * 0.20;                         // spend <=20%
            double spend = (maxSpend > 0) ? randBetween(maxSpend * 0.10, maxSpend) : 0.0;
            double amtBase = (bidPrice > 0) ? (spend / bidPrice) : 0.0;

            if (amtBase > 0) {
                OrderBookEntry bid{bidPrice, amtBase, nowTs, product, OrderBookType::bid, username};
                if (wallet.canFulfillOrder(bid)) {
                    orderBook.insertOrder(bid);

                    Transaction t{
                        username,
                        txManager.nowTimestamp(),
                        "BID",
                        product,
                        quote,
                        spend, // record "money spent" in quote currency
                        wallet.getWalletBalanceForCurrency(quote)
                    };
                    txManager.append(t);
                }
            }
        }
    }
}

void TradeSimulator::simulateForAllProducts(OrderBook& orderBook,
                                            Wallet& wallet,
                                            TransactionManager& txManager,
                                            const std::string& username,
                                            int ordersPerSidePerProduct)
{
    std::srand((unsigned int) std::time(nullptr));

    auto products = orderBook.getKnownProducts();
    addBalanceToWalletIfNeeded(products, wallet, txManager, username);

    std::string nowTs = txManager.nowTimestamp();

    // Use last dataset timestamp as price reference, then replicate liquidity at nowTs
    std::string lastTs = orderBook.getLatestTime();
    addMarketLiquidityAtTimestamp(orderBook, products, lastTs, nowTs);

    // Now generate user orders at nowTs
    for (const std::string& product : products) {
        createOrdersForProduct(orderBook, wallet, txManager, username, product, nowTs, ordersPerSidePerProduct);
    }
}
