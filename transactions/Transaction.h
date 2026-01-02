#pragma once
#include <string>

class Transaction {
public:
    Transaction();

    Transaction(const std::string& username,
                const std::string& timestamp,
                const std::string& action,
                const std::string& product,
                const std::string& currency,
                const double& amount,
                const double& balanceAfter);

    std::string username;
    std::string timestamp;
    std::string action;
    std::string product;
    std::string currency;
    double amount;
    double balanceAfter;
};
