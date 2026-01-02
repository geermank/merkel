cmake_minimum_required(VERSION 3.16)
project(untitled)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(untitled main.cpp
        MerkelMain.cpp
        wallet/Wallet.cpp
        OrderBook.cpp
        OrderBookEntry.cpp
        CSVReader.cpp
        auth/UserManager.h
        auth/User.h
        auth/User.cpp
        auth/UserManager.cpp
        candlestick/Candlestick.h
        candlestick/Candlestick.cpp
        candlestick/CandlestickCalculator.h
        candlestick/CandlestickCalculator.cpp
        transactions/Transaction.h
        transactions/Transaction.cpp
        transactions/TransactionManager.h
        transactions/TransactionManager.cpp
        wallet/WalletManager.h
        wallet/WalletManager.cpp
        wallet/WalletRecord.h
        wallet/WalletRecord.cpp
        simulation/TradeSimulator.h
        simulation/TradeSimulator.cpp)
