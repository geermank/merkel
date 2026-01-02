#pragma once

#include <string>
#include <map>
#include <iostream>
#include "../OrderBookEntry.h"
#include "WalletRecord.h"

class Wallet 
{
    public:
        Wallet();
        /** insert currency to the wallet */
        void insertCurrency(std::string type, double amount);
        /** remove currency from the wallet */
        bool removeCurrency(std::string type, double amount);
        
        /** check if the wallet contains this much currency or more */
        bool containsCurrency(std::string type, double amount);
        /** checks if the wallet can cope with this ask or bid.*/
        bool canFulfillOrder(OrderBookEntry order);
        /** update the contents of the wallet
         * assumes the order was made by the owner of the wallet
        */
        void processSale(OrderBookEntry& sale);
        double getWalletBalanceForCurrency(std::string type);

        /**
         * Converts the currencies in this wallet into WalletRecord instances
         * @param username the current logged in user
         * @return the currencies and its amounts as walletRecord
         */
        std::vector<WalletRecord> toWalletRecords(const std::string& username);

        /** generate a string representation of the wallet */
        std::string toString();
        friend std::ostream& operator<<(std::ostream& os, Wallet& wallet);

        
    private:
        std::map<std::string,double> currencies;

};



	

