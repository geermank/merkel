#pragma once

#include <string>
#include <vector>
#include "../OrderBook.h"
#include "../wallet/Wallet.h"
#include "../transactions/TransactionManager.h"

class TradeSimulator {
public:
    // Generates >=5 bids and >=5 asks per product, timestamp = nowTs
    static void simulateForAllProducts(OrderBook& orderBook,
                                       Wallet& wallet,
                                       TransactionManager& txManager,
                                       const std::string& username,
                                       int ordersPerSidePerProduct = 5);

private:
    static double randBetween(double a, double b);

    static void addBalanceToWalletIfNeeded(const std::vector<std::string>& products,
                                   Wallet& wallet,
                                   TransactionManager& txManager,
                                   const std::string& username);

    static void addMarketLiquidityAtTimestamp(OrderBook& orderBook,
                                              const std::vector<std::string>& products,
                                              const std::string& sourceTimestamp,
                                              const std::string& newTimestamp);

    static double computeMidPrice(OrderBook& orderBook,
                                  const std::string& product,
                                  const std::string& timestamp);

    static void createOrdersForProduct(OrderBook& orderBook,
                                       Wallet& wallet,
                                       TransactionManager& txManager,
                                       const std::string& username,
                                       const std::string& product,
                                       const std::string& nowTs,
                                       int n);
};
