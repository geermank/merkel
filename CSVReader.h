#pragma once
#include <vector>
#include <string>
#include "auth/User.h"
#include "OrderBookEntry.h"

class CSVReader
{
    public:
     CSVReader();

     static std::vector<OrderBookEntry> readCSV(const std::string& csvFilename);
     static std::vector<User> readUsersCSV(const std::string& csvFilename);

     static std::vector<std::string> tokenise(std::string csvLine, char separator);
    
     static OrderBookEntry stringsToOBE(std::string price, 
                                        std::string amount, 
                                        std::string timestamp, 
                                        std::string product, 
                                        OrderBookType OrderBookType);

    static std::string userToCSV(const User& user);

    private:
     static OrderBookEntry stringsToOBE(std::vector<std::string> strings);
     
};
