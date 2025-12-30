cmake_minimum_required(VERSION 3.16)
project(untitled)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_executable(untitled main.cpp
        MerkelMain.cpp
        Wallet.cpp
        OrderBook.cpp
        OrderBookEntry.cpp
        CSVReader.cpp
        auth/UserManager.h
        auth/User.h
        auth/User.cpp
        auth/UserManager.cpp)
