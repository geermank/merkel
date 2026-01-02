#include "TradeSimulator.h"
#include "../CSVReader.h"
#include <cstdlib>
#include <algorithm>

double TradeSimulator::randBetween(double low, double high) {
    // I got this logic from:
    // https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c
    double r = (double) std::rand() / (double) RAND_MAX;
    return low + r * (high - low);
}

void TradeSimulator::addCurrenciesToWallet(const std::vector<std::string>& products,
                                        Wallet& wallet,
                                        TransactionManager& txManager,
                                        const std::string& username)
{
    // deposit a fixed amount of each currency to guarantee orders will always execute
    std::map<std::string, bool> currencies;

    for (const std::string& p : products) {
        std::vector<std::string> parts = CSVReader::tokenise(p, '/');
        if (parts.size() == 2) {
            currencies[parts[0]] = true;
            currencies[parts[1]] = true;
        }
    }

    for (const std::pair<const std::string, bool>& pair : currencies) {
        const std::string& currency = pair.first;

        wallet.insertCurrency(currency, 10.0);

        Transaction t{
            username,
            txManager.nowTimestamp(),
            "DEPOSIT",
            currency,
            currency,
            10.0,
            wallet.getWalletBalanceForCurrency(currency)
        };
        txManager.append(t);
    }
}

void TradeSimulator::addMarketOrdersAtTimestamp(OrderBook& orderBook,
                                                   const std::vector<std::string>& products,
                                                   const std::string& sourceTimestamp,
                                                   const std::string& newTimestamp)
{
    for (const std::string& product : products) {
        std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product, sourceTimestamp);
        std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product, sourceTimestamp);

        if (asks.empty()) asks = orderBook.getOrders(OrderBookType::ask, product);
        if (bids.empty()) bids = orderBook.getOrders(OrderBookType::bid, product);

        std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
        std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

        int depth = 10;
        for (int i = 0; i < (int) asks.size() && i < depth; i++) {
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
    std::vector<std::string> parts = CSVReader::tokenise(product, '/');
    if (parts.size() != 2) return;

    std::string p1 = parts[0];
    std::string p2 = parts[1];

    std::vector<OrderBookEntry> asksNow = orderBook.getOrders(OrderBookType::ask, product, nowTs);
    std::vector<OrderBookEntry> bidsNow = orderBook.getOrders(OrderBookType::bid, product, nowTs);
    if (asksNow.empty() || bidsNow.empty()) return;

    double bestAsk = OrderBook::getLowPrice(asksNow);
    double bestBid = OrderBook::getHighPrice(bidsNow);

    for (int i = 0; i < n; i++) {

        double p1Bal = wallet.getWalletBalanceForCurrency(p1);
        double maxAmt = p1Bal * 0.20;
        double amt = randBetween(maxAmt * 0.10, maxAmt);
        double askPrice = bestBid * (1.0 - randBetween(0.0005, 0.002));

        OrderBookEntry ask{askPrice, amt, nowTs, product, OrderBookType::ask, username};
        if (wallet.canFulfillOrder(ask)) {
            orderBook.insertOrder(ask);
            Transaction t{
                username,
                txManager.nowTimestamp(),
                "ASK",
                product,
                p1,
                amt,
                wallet.getWalletBalanceForCurrency(p1)
            };
            txManager.append(t);
        }

        double p2Bal = wallet.getWalletBalanceForCurrency(p2);
        double bidPrice = bestAsk * (1.0 + randBetween(0.0005, 0.002));
        double maxSpend = p2Bal * 0.20;
        double spend = randBetween(maxSpend * 0.10, maxSpend);
        double amtBase = spend / bidPrice;

        OrderBookEntry bid{bidPrice, amtBase, nowTs, product, OrderBookType::bid, username};
        if (wallet.canFulfillOrder(bid)) {
            orderBook.insertOrder(bid);
            Transaction t{
                username,
                txManager.nowTimestamp(),
                "BID",
                product,
                p2,
                spend,
                wallet.getWalletBalanceForCurrency(p2)
            };
            txManager.append(t);
        }
    }
}

std::string TradeSimulator::simulateForAllProducts(OrderBook& orderBook,
                                            Wallet& wallet,
                                            TransactionManager& txManager,
                                            const std::string& username,
                                            int ordersPerSidePerProduct)
{
    std::vector<std::string> products = orderBook.getKnownProducts();
    addCurrenciesToWallet(products, wallet, txManager, username);

    std::string nowTs = txManager.nowTimestamp();
    std::string lastTs = orderBook.getLatestTime();
    addMarketOrdersAtTimestamp(orderBook, products, lastTs, nowTs);

    for (const std::string& product : products) {
        createOrdersForProduct(orderBook, wallet, txManager, username, product, nowTs, ordersPerSidePerProduct);
    }

    return nowTs;
}
