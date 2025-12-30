#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "auth/UserManager.h"
#include "auth/User.h"
#include "candlestick/CandlestickCalculator.h"
#include "candlestick/Candlestick.h"

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


        void printCandlestickSummary();
        CandlestickGranularity askGranularity();
        void printCandles(const std::string& type,
              const std::string& product,
              const CandlestickGranularity& granularity,
              const std::vector<Candlestick>& candles);

        void printLoginMenu();
        void handleRegister();
        void handleLogin();
        void handleResetPassword();
        void logout();

        std::string currentTime;

        // OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook {"/Users/german/CLionProjects/untitled/20200601.csv"};
        UserManager userManager{"/Users/german/CLionProjects/untitled/users.csv"};

        Wallet wallet;
        User currentUser;

        bool isLoggedIn = false;
};
