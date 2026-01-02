#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "wallet/Wallet.h"
#include "auth/UserManager.h"
#include "auth/User.h"
#include "candlestick/CandlestickCalculator.h"
#include "candlestick/Candlestick.h"
#include "wallet/WalletManager.h"
#include "transactions/TransactionManager.h"
#include "simulation/TradeSimulator.h"

class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();
    private:
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        void processUserOption(int userOption);
        void depositMoney();
        void withdrawMoney();
        void printRecentTransactions();
        void printUserStats();
        void simulateUserTrading();


        void printCandlestickSummary();
        CandlestickGranularity askGranularity();
        void printCandles(const std::string& type,
              const std::string& product,
              const CandlestickGranularity& granularity,
              const std::vector<Candlestick>& candles);

        void loadWallet();

        void printLoginMenu();
        void handleRegister();
        void handleLogin();
        void handleResetPassword();
        void logout();

        std::string currentTime;

        // OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook {"/Users/german/CLionProjects/untitled/20200601.csv"};
        UserManager userManager{"/Users/german/CLionProjects/untitled/users.csv"};
        WalletManager walletManager{"/Users/german/CLionProjects/untitled/wallets.csv"};
        TransactionManager transactionManager{"/Users/german/CLionProjects/untitled/transactions.csv"};

        Wallet wallet;
        User currentUser;

        bool isLoggedIn = false;
};
