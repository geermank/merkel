#include "WalletRecord.h"

WalletRecord::WalletRecord()
    : username(""), currency(""), amount(0) {}

WalletRecord::WalletRecord(std::string username,
                           std::string currency,
                           double amount)
    : username(username), currency(currency), amount(amount) {}
