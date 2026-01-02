#pragma once
#include <string>
#include <vector>
#include "Transaction.h"

class TransactionManager {
public:
    TransactionManager(std::string transactionsCsvPath);

    /** registers a new transaction */
    void append(Transaction t);

    /** returns last n transactions */
    std::vector<Transaction> lastN(const std::string& username, int n);
    /** returns last n transactions of the given product */
    std::vector<Transaction> lastNByProduct(const std::string& username, const std::string& product, int n);

    /**
     * returns the number of transactions of a certain action (like BID, ASK, etc.).
     * It can be filtered by product, but this is optional, the productFilter can be empty
     */
    int countActions(std::string username, std::string action, std::string productFilter);
    double totalSpentInTimeframe(std::string username,
                                 std::string startTimestamp,
                                 std::string endTimestamp,
                                 std::string quoteCurrencyFilter);

    /** generates a timestamp string of the current system time */
    std::string nowTimestamp();

private:
    std::string transactionsCsvPath;
    std::vector<Transaction> transactions;

    /** returns true if the given timestamp ts is between the start and end timestamps */
    bool isBetween(std::string ts, std::string startTs, std::string endTs);
};
