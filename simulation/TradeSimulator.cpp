#include "TradeSimulator.h"
#include "../CSVReader.h"
#include <cstdlib>
#include <algorithm>

double TradeSimulator::randBetween(double low, double high) {
    // NOTE: I got this logic from:
    // https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c
    double r = (double) std::rand() / (double) RAND_MAX;
    return low + r * (high - low);
}

void TradeSimulator::addCurrenciesToWallet(const std::vector<std::string>& products,
                                        Wallet& wallet,
                                        TransactionManager& txManager,
                                        const std::string& username)
{
    // create a set of currencies, without duplicates
    std::map<std::string, bool> currencies;
    for (const std::string& p : products) {
        std::vector<std::string> parts = CSVReader::tokenise(p, '/');
        if (parts.size() == 2) {
            currencies[parts[0]] = true;
            currencies[parts[1]] = true;
        }
    }

    // traverse the map of currencies and for each of them add funds into the user's wallet
    for (const std::pair<const std::string, bool>& pair : currencies) {
        const std::string& currency = pair.first;

        wallet.insertCurrency(currency, 10.0);

        // register the operation as a deposit in the transaction manager
        Transaction t {
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
        // get all the asks and bids at the source timestamp
        // to use these orders as a starting point for the new market orders
        // copying the base values like amount and price, but changing its timestamp and username
        std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product, sourceTimestamp);
        std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product, sourceTimestamp);
        // as a fallback solution, if there are no bids or asks at that timestamp
        // get all orders for this product, without the timestamp filter
        if (asks.empty()) asks = orderBook.getOrders(OrderBookType::ask, product);
        if (bids.empty()) bids = orderBook.getOrders(OrderBookType::bid, product);

        // ensure best bid and ask are matched first
        std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
        std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

        // create 10 bids and 10 asks for each product
        int depth = 10;
        for (int i = 0; i < asks.size() && i < depth; i++) {
            OrderBookEntry e{asks[i].price, asks[i].amount, newTimestamp, product, OrderBookType::ask, "dataset"};
            orderBook.insertOrder(e);
        }
        for (int i = 0; i < bids.size() && i < depth; i++) {
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
                                            const std::string& nowTimestamp,
                                            int n)
{
    std::vector<std::string> parts = CSVReader::tokenise(product, '/');
    if (parts.size() != 2) return;

    const std::string& p1 = parts[0];
    const std::string& p2 = parts[1];

    // get current (at nowTimestamp) bids and asks
    std::vector<OrderBookEntry> asksNow = orderBook.getOrders(OrderBookType::ask, product, nowTimestamp);
    std::vector<OrderBookEntry> bidsNow = orderBook.getOrders(OrderBookType::bid, product, nowTimestamp);
    if (asksNow.empty() || bidsNow.empty()) return;
    // and use them to get the best current prices
    double bestAsk = OrderBook::getLowPrice(asksNow);
    double bestBid = OrderBook::getHighPrice(bidsNow);

    // we will create n orders for each product
    for (int i = 0; i < n; i++) {
        // choose a random ask size based on a fraction of the available balance
        // and price it close to the current best bid to increase the chance of a match
        double p1Bal = wallet.getWalletBalanceForCurrency(p1);
        double maxAmt = p1Bal * 0.20;
        double amt = randBetween(maxAmt * 0.10, maxAmt);
        double askPrice = bestBid * (1.0 - randBetween(0.0005, 0.002));

        OrderBookEntry ask{ askPrice, amt, nowTimestamp, product, OrderBookType::ask, username};
        if (wallet.canFulfillOrder(ask)) {
            orderBook.insertOrder(ask);

            // create the ASK transaction in the registry
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

        // generate a bid order based on available balance and current market price
        double p2Bal = wallet.getWalletBalanceForCurrency(p2);
        double bidPrice = bestAsk * (1.0 + randBetween(0.0005, 0.002));
        double maxSpend = p2Bal * 0.20;
        double spend = randBetween(maxSpend * 0.10, maxSpend);
        double amtBase = spend / bidPrice;

        OrderBookEntry bid{bidPrice, amtBase, nowTimestamp, product, OrderBookType::bid, username};
        if (wallet.canFulfillOrder(bid)) {
            orderBook.insertOrder(bid);

            // create the BID transaction in the registry
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
    // add funds to the wallet of the current user
    // to guarantee that orders will be executed
    std::vector<std::string> products = orderBook.getKnownProducts();
    addCurrenciesToWallet(products, wallet, txManager, username);

    // simulate other market orders not generated by the current user
    // we'll use these orders later as a reference to calculate user order prices
    // and make sure we'll have matches and sales
    std::string nowTs = txManager.nowTimestamp();
    std::string lastTs = orderBook.getLatestTime();
    addMarketOrdersAtTimestamp(orderBook, products, lastTs, nowTs);

    // create user orders for every product in the order book
    for (const std::string& product : products) {
        createOrdersForProduct(orderBook, wallet, txManager, username, product, nowTs, ordersPerSidePerProduct);
    }

    return nowTs;
}
