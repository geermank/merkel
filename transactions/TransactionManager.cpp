#include "TransactionManager.h"
#include "../CSVReader.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

TransactionManager::TransactionManager(std::string transactionsCsvPath)
    : transactionsCsvPath(transactionsCsvPath)
{
    transactions = CSVReader::readTransactionsCSV(transactionsCsvPath);
}

void TransactionManager::append(Transaction t)
{
    transactions.push_back(t);

    std::ofstream file(transactionsCsvPath, std::ios::app);
    if (!file.is_open()) {
        std::cout << "TransactionManager::append cannot open file" << std::endl;
        throw std::exception();
    }
    file << CSVReader::transactionToCSV(t) << std::endl;
}

std::vector<Transaction> TransactionManager::lastN(const std::string& username, int n)
{
    std::vector<Transaction> result;
    // iterate the transactions vector backwards to get the last n registered transactions
    for (int i = (int) transactions.size() - 1; i >= 0 && result.size() < n; i--) {
        if (transactions[i].username == username) {
            result.push_back(transactions[i]);
        }
    }
    return result;
}

std::vector<Transaction> TransactionManager::lastNByProduct(const std::string& username,
                                                            const std::string& product,
                                                            int n)
{
    std::vector<Transaction> result;
    // iterate the transactions vector backwards to get the last n registered transactions
    // filtering by the given product
    for (int i = (int) transactions.size() - 1; i >= 0 && result.size() < n; i--) {
        if (transactions[i].username == username && transactions[i].product == product) {
            result.push_back(transactions[i]);
        }
    }

    return result;
}

int TransactionManager::countActions(std::string username, std::string action, std::string productFilter)
{
    int count = 0;
    for (const Transaction& t : transactions) {
        if (t.username != username) continue;
        if (t.action != action) continue;
        if (!productFilter.empty() && t.product != productFilter) continue;
        count++;
    }
    return count;
}

bool TransactionManager::isBetween(std::string ts, std::string startTs, std::string endTs)
{
    if (!startTs.empty() && ts < startTs) return false;
    if (!endTs.empty() && ts > endTs) return false;
    return true;
}

double TransactionManager::totalSpentInTimeframe(std::string username,
                                                 std::string startTimestamp,
                                                 std::string endTimestamp,
                                                 std::string currency)
{
    double total = 0;
    for (const Transaction& t : transactions) {
        // check if this transaction applies for contabilization
        if (t.username != username) continue;
        if (!isBetween(t.timestamp, startTimestamp, endTimestamp)) continue;

        if (t.action == "BID" || t.action == "ASK") {
            // filter by currency if required, although this is an optional param
            if (currency.empty() || t.currency == currency) {
                // only take into account the bids made by this user
                if (t.action == "BID") total += t.amount;
            }
        }
    }
    return total;
}

std::string TransactionManager::nowTimestamp()
{
    // honestly, I used AI help for this method cause I wasn't sure how
    // to generate this string, and couldn't find any helpful resources on the Internet
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);

    std::ostringstream oss;
    oss << std::setfill('0')
        << (tm.tm_year + 1900) << "/"
        << std::setw(2) << (tm.tm_mon + 1) << "/"
        << std::setw(2) << tm.tm_mday << " "
        << std::setw(2) << tm.tm_hour << ":"
        << std::setw(2) << tm.tm_min << ":"
        << std::setw(2) << tm.tm_sec;

    return oss.str();
}
