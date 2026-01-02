#pragma once
#include <string>
#include "Wallet.h"

class WalletManager {
public:
    WalletManager(std::string walletsCsvPath);

    void loadWallet(const std::string& username, Wallet& wallet);
    void saveWallet(const std::string& username, Wallet& wallet);
private:
    std::string walletsCsvPath;
    void removeUserLines(const std::string& username, std::vector<std::string>& lines);
};
