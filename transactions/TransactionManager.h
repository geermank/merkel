#pragma once
#include <string>
#include <vector>
#include "Transaction.h"

class TransactionManager {
public:
    TransactionManager(std::string transactionsCsvPath);

    void append(Transaction t);

    std::vector<Transaction> lastN(const std::string& username, int n);
    std::vector<Transaction> lastNByProduct(const std::string& username, const std::string& product, int n);

    int countActions(std::string username, std::string action, std::string productFilter);
    double totalSpentInTimeframe(std::string username,
                                 std::string startTimestamp,
                                 std::string endTimestamp,
                                 std::string quoteCurrencyFilter);

    std::string nowTimestamp();

private:
    std::string transactionsCsvPath;
    std::vector<Transaction> transactions;

    bool isBetween(std::string ts, std::string startTs, std::string endTs);
};
