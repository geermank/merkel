#include "WalletManager.h"
#include "../CSVReader.h"
#include "WalletRecord.h"
#include <fstream>
#include <iostream>

WalletManager::WalletManager(std::string walletsCsvPath)
    : walletsCsvPath(walletsCsvPath) {}

void WalletManager::loadWallet(const std::string& username, Wallet& wallet)
{
    const std::vector<WalletRecord> records = CSVReader::readWalletsCSV(walletsCsvPath);
    for (const WalletRecord& r : records) {
        if (r.username == username) {
            wallet.insertCurrency(r.currency, r.amount);
        }
    }
}

void WalletManager::removeUserLines(const std::string& username, std::vector<std::string>& lines)
{
    std::vector<std::string> filtered;

    for (std::string l : lines) {
        std::vector<std::string> tokens = CSVReader::tokenise(l, ',');
        if (tokens.size() != 3) continue;
        if (tokens[0] != username) filtered.push_back(l);
    }

    lines = filtered;
}

void WalletManager::saveWallet(const std::string& username, Wallet& wallet)
{
    std::vector<std::string> lines;
    {
        std::ifstream in(walletsCsvPath);
        if (in.is_open()) {
            std::string line;
            while (std::getline(in, line)) {
                if (!line.empty()) lines.push_back(line);
            }
        } else {
            std::cout << "WalletManager::saveWallet cannot open file" << std::endl;
        }
    }

    // remove previous user records, as we are adding them again but updated
    removeUserLines(username, lines);

    // get latest state from the wallet and convert it into CSV lines
    std::vector<WalletRecord> walletRecords = wallet.toWalletRecords(username);
    for (const WalletRecord& r : walletRecords) {
        lines.push_back(CSVReader::walletRecordToCSV(r));
    }

    std::ofstream out(walletsCsvPath, std::ios::trunc);
    if (!out.is_open()) {
        std::cout << "WalletManager::saveWallet cannot open file" << std::endl;
        throw std::exception();
    }

    for (const std::string& l : lines) {
        out << l << std::endl;
    }
}
