#pragma once

#include <string>
#include <vector>
#include "../OrderBook.h"
#include "../wallet/Wallet.h"
#include "../transactions/TransactionManager.h"

class TradeSimulator {
public:
    /**
     * Simulates user trading by automatically generating user orders for every
     * product in the given order book
     *
     * @param orderBook where we'll get orders from and create into
     * @param wallet the user's wallet to fulfill the orders
     * @param txManager to register the order creation as transactions
     * @param username that is currently logged in
     * @param ordersPerSidePerProduct the number of orders to create per product
     *
     * @return the timestamp of the new orders
     */
    static std::string simulateForAllProducts(OrderBook& orderBook,
                                              Wallet& wallet,
                                              TransactionManager& txManager,
                                              const std::string& username,
                                              int ordersPerSidePerProduct);

private:
    /** generates a random double between the given boundaries */
    static double randBetween(double a, double b);

    /** adds funds in the wallet for every given product */
    static void addCurrenciesToWallet(const std::vector<std::string>& products,
                                   Wallet& wallet,
                                   TransactionManager& txManager,
                                   const std::string& username);

    /** creates system orders in the orderbook for the given products and at the new timestamp */
    static void addMarketOrdersAtTimestamp(OrderBook& orderBook,
                                              const std::vector<std::string>& products,
                                              const std::string& sourceTimestamp,
                                              const std::string& newTimestamp);

    /** creates n user orders for the given product */
    static void createOrdersForProduct(OrderBook& orderBook,
                                       Wallet& wallet,
                                       TransactionManager& txManager,
                                       const std::string& username,
                                       const std::string& product,
                                       const std::string& nowTimestamp,
                                       int n);
};
