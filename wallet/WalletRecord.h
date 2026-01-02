#pragma once
#include <string>

class WalletRecord {
public:
    WalletRecord();

    WalletRecord(std::string username,
                 std::string currency,
                 double amount);

    std::string username;
    std::string currency;
    double amount;
};
