#pragma once

#include <string>
#include <vector>
#include "../OrderBook.h"
#include "../wallet/Wallet.h"
#include "../transactions/TransactionManager.h"

class TradeSimulator {
public:
    static static std::string simulateForAllProducts(OrderBook& orderBook,
                                       Wallet& wallet,
                                       TransactionManager& txManager,
                                       const std::string& username,
                                       int ordersPerSidePerProduct);

private:
    static double randBetween(double a, double b);

    static void addCurrenciesToWallet(const std::vector<std::string>& products,
                                   Wallet& wallet,
                                   TransactionManager& txManager,
                                   const std::string& username);

    static void addMarketOrdersAtTimestamp(OrderBook& orderBook,
                                              const std::vector<std::string>& products,
                                              const std::string& sourceTimestamp,
                                              const std::string& newTimestamp);

    static void createOrdersForProduct(OrderBook& orderBook,
                                       Wallet& wallet,
                                       TransactionManager& txManager,
                                       const std::string& username,
                                       const std::string& product,
                                       const std::string& nowTs,
                                       int n);
};
