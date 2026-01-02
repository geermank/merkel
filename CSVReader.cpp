#include "CSVReader.h"
#include "auth/User.h"
#include <iostream>
#include <fstream>

CSVReader::CSVReader()
{

}

std::string CSVReader::userToCSV(const User& user)
{
    return user.getUsername() + "," +
        user.getFullName() + "," +
        user.getEmail() + "," +
        user.getPasswordHash();
}

std::vector<User> CSVReader::readUsersCSV(const std::string& csvFilename)
{
    std::vector<User> users;
    std::ifstream file(csvFilename);
    if (!file.is_open()) return users;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = tokenise(line, ',');
        if (tokens.size() != 4) {
            std::cout << "CSVReader::readUsersCSV bad data" << std::endl;
            continue;
        };

        User u{tokens[0], tokens[1], tokens[2], tokens[3]};
        users.push_back(u);
    }
    return users;
}

std::vector<Transaction> CSVReader::readTransactionsCSV(const std::string& csvFilename)
{
    std::vector<Transaction> transactions;
    std::ifstream file(csvFilename);
    if (!file.is_open()) {
        std::cout << "CSVReader::readTransactionsCSV couldn't open file" << std::endl;
        return transactions;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = tokenise(line, ',');
        if (tokens.size() != 7) {
            std::cout << "CSVReader::readTransactionsCSV bad data" << std::endl;
            continue;
        }

        try {
            double amount = std::stod(tokens[5]);
            double balanceAfter = std::stod(tokens[6]);

            Transaction t{
                tokens[0],
                tokens[1],
                tokens[2],
                tokens[3],
                tokens[4],
                amount,
                balanceAfter
            };
            transactions.push_back(t);
        } catch (...) {
            std::cout << "CSVReader::readTransactionsCSV bad number" << std::endl;
        }
    }

    return transactions;
}

std::vector<WalletRecord> CSVReader::readWalletsCSV(const std::string& csvFilename)
{
    std::vector<WalletRecord> records;

    std::ifstream file(csvFilename);
    if (!file.is_open()) return records;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = tokenise(line, ',');
        if (tokens.size() != 3) {
            std::cout << "CSVReader::readWalletsCSV bad data" << std::endl;
            continue;
        }

        try {
            double amount = std::stod(tokens[2]);
            WalletRecord r(tokens[0], tokens[1], amount);
            records.push_back(r);
        } catch (...) {
            std::cout << "CSVReader::readWalletsCSV bad number" << std::endl;
        }
    }

    return records;
}

std::vector<OrderBookEntry> CSVReader::readCSV(const std::string& csvFilename)
{
    std::vector<OrderBookEntry> entries;

    std::ifstream csvFile{csvFilename};
    std::string line;

    if (csvFile.is_open())
    {
        while(std::getline(csvFile, line))
        {
            try {
                OrderBookEntry obe = stringsToOBE(tokenise(line, ','));
                entries.push_back(obe);
            }catch(const std::exception& e)
            {
                std::cout << "CSVReader::readCSV bad data"  << std::endl;
            }
        }// end of while
    }    

    std::cout << "CSVReader::readCSV read " << entries.size() << " entries"  << std::endl;
    return entries; 
}

std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator)
{
   std::vector<std::string> tokens;
   signed int start, end;
   std::string token;
    start = csvLine.find_first_not_of(separator, 0);
    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) break;
        if (end >= 0) token = csvLine.substr(start, end - start);
        else token = csvLine.substr(start, csvLine.length() - start);
        tokens.push_back(token);
    start = end + 1;
    }while(end > 0);

   return tokens; 
}

OrderBookEntry CSVReader::stringsToOBE(std::vector<std::string> tokens)
{
    double price, amount;

    if (tokens.size() != 5) // bad
    {
        std::cout << "Bad line " << std::endl;
        throw std::exception{};
    }
    // we have 5 tokens
    try {
         price = std::stod(tokens[3]);
         amount = std::stod(tokens[4]);
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[3]<< std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << tokens[4]<< std::endl; 
        throw;        
    }

    OrderBookEntry obe{price, 
                        amount, 
                        tokens[0],
                        tokens[1], 
                        OrderBookEntry::stringToOrderBookType(tokens[2])};

    return obe; 
}


OrderBookEntry CSVReader::stringsToOBE(std::string priceString, 
                                    std::string amountString, 
                                    std::string timestamp, 
                                    std::string product, 
                                    OrderBookType orderType)
{
    double price, amount;
    try {
         price = std::stod(priceString);
         amount = std::stod(amountString);
    }catch(const std::exception& e){
        std::cout << "CSVReader::stringsToOBE Bad float! " << priceString<< std::endl;
        std::cout << "CSVReader::stringsToOBE Bad float! " << amountString<< std::endl; 
        throw;        
    }
    OrderBookEntry obe{price, 
                    amount, 
                    timestamp,
                    product, 
                    orderType};
                
    return obe;
}

std::string CSVReader::transactionToCSV(const Transaction &t) {
    return t.username + "," +
       t.timestamp + "," +
       t.action + "," +
       t.product + "," +
       t.currency + "," +
       std::to_string(t.amount) + "," +
       std::to_string(t.balanceAfter);
}

std::string CSVReader::walletRecordToCSV(const WalletRecord& wr)
{
    return wr.username + "," + wr.currency + "," + std::to_string(wr.amount);
}
