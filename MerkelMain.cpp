#include <iostream>
#include <vector>
#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    while (true) {
        while (!isLoggedIn) {
            printLoginMenu();
        }

        currentTime = orderBook.getEarliestTime();

        while (isLoggedIn) {
            printMenu();
            int input = getUserOption();
            processUserOption(input);
        }
    }
}


void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;
    // 7 Candlestick summary
    std::cout << "7: Candlestick summary " << std::endl;
    // 8 Deposit
    std::cout << "8: Deposit " << std::endl;
    // 9 Withdraw
    std::cout << "9: Withdraw " << std::endl;
    // 10: Recent transactions
    std::cout << "10: Recent transactions " << std::endl;
    // 11: User stats
    std::cout << "11: User stats" << std::endl;
    // 12: Simulate trading
    std::cout << "12: Simulate trading" << std::endl;
    // 13 logout
    std::cout << "13: Logout" << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }
    // std::cout << "OrderBook contains :  " << orders.size() << " entries" << std::endl;
    // unsigned int bids = 0;
    // unsigned int asks = 0;
    // for (OrderBookEntry& e : orders)
    // {
    //     if (e.orderType == OrderBookType::ask)
    //     {
    //         asks ++;
    //     }
    //     if (e.orderType == OrderBookType::bid)
    //     {
    //         bids ++;
    //     }  
    // }    
    // std::cout << "OrderBook asks:  " << asks << " bids:" << bids << std::endl;

}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << "Your Wallet" << std::endl;
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price
                      << " amount " << sale.amount
                      << " type " << OrderBookEntry::orderBookTypeToString(sale.orderType)
                      << " user " << sale.username
                      << std::endl;
            if (sale.username == currentUser.getUsername())
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
    }

    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::logout() {
    std::cout << "Goodbye!" << std::endl;

    walletManager.saveWallet(currentUser.getUsername(), wallet);
    wallet = Wallet{};

    isLoggedIn = false;
    currentUser = User{};
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-13" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        //system("clear");
        std::cout << "Invalid choice. Choose 1-8" << std::endl;
        std::cout << "==========================" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }
    if (userOption == 7)
    {
        printCandlestickSummary();
    }
    if (userOption == 8)
    {
        depositMoney();
    }
    if (userOption == 9)
    {
        withdrawMoney();
    }
    if (userOption == 10)
    {
        printRecentTransactions();
    }
    if (userOption == 11)
    {
        printUserStats();
    }
    if (userOption == 12)
    {
        simulateUserTrading();
    }
    if (userOption == 13)
    {
        logout();
    }
}
void MerkelMain::printLoginMenu()
{
    std::cout << "Welcome! Who are you?" << std::endl;
    std::cout << "1) Register" << std::endl;
    std::cout << "2) Login" << std::endl;
    std::cout << "3) Reset password" << std::endl;
    std::cout << "Choose an option: ";

    std::string line;
    std::getline(std::cin, line);

    int op = 0;
    try {
        op = std::stoi(line);
    } catch(const std::exception&) {
        op = 0;
    }

    if (op == 1) handleRegister();
    else if (op == 2) handleLogin();
    else if (op == 3) handleResetPassword();
    else std::cout << "Invalid option\n";
}

void MerkelMain::handleRegister()
{
    std::cout << "Let's create a new user. Please, complete the form below when requested" << std::endl;
    std::string fullName, email, password;
    std::cout << "Full name: ";
    std::getline(std::cin, fullName);
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    try {
        currentUser = userManager.registerUser(fullName, email, password);
        isLoggedIn = true;
        loadWallet();
        std::cout << "Registered. Your username is: " << currentUser.getUsername() << std::endl;
    } catch (...) {
        std::cout << "Register failed\n";
    }
}

void MerkelMain::handleLogin()
{
    std::cout << "Let's log you in" << std::endl;
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    try {
        currentUser = userManager.login(username, password);
        isLoggedIn = true;
        loadWallet();
        std::cout << "Logged in successfully. Your username is: " << currentUser.getUsername() << std::endl;
    } catch (std::exception&) {
        std::cout << "Login failed." << std::endl;
    }
}

void MerkelMain::handleResetPassword()
{
    std::cout << "Let's reset your password" << std::endl;
    std::string fullName, email, newPassword;
    std::cout << "Full name: ";
    std::getline(std::cin, fullName);
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "New password: ";
    std::getline(std::cin, newPassword);

    try {
        if (userManager.resetPassword(fullName, email, newPassword)) {
            std::cout << "Password updated" << std::endl;
        } else {
            std::cout << "Reset failed" << std::endl;
        }
    } catch (std::exception&) {
        std::cout << "Reset failed" << std::endl;
    }
}

CandlestickGranularity MerkelMain::askGranularity()
{
    std::cout << "Choose granularity" << std::endl;
    std::cout << "1. Daily" << std::endl;
    std::cout << "2. Monthly" << std::endl;
    std::cout << "3. Yearly" << std::endl;
    std::cout << "Press Enter for default (yearly): ";

    std::string line;
    std::getline(std::cin, line);

    // do not cast for simplicity. any other scenario falls under yearly granularity
    if (line == "1") return CandlestickGranularity::Daily;
    if (line == "2") return CandlestickGranularity::Monthly;
    if (line == "3") return CandlestickGranularity::Yearly;
    return CandlestickGranularity::Yearly;
}

void MerkelMain::printCandlestickSummary()
{
    std::cout << "Enter product (example: ETH/BTC): ";
    std::string product;
    std::getline(std::cin, product);

    CandlestickGranularity granularity = askGranularity();

    std::vector<OrderBookEntry> asks = orderBook.getOrders(OrderBookType::ask, product);
    std::vector<Candlestick> askCandles = CandlestickCalculator::buildCandlesticks(asks, product, OrderBookType::ask, granularity);
    printCandles("ASK", product, granularity, askCandles);

    std::vector<OrderBookEntry> bids = orderBook.getOrders(OrderBookType::bid, product);
    std::vector<Candlestick> bidCandles = CandlestickCalculator::buildCandlesticks(bids, product, OrderBookType::bid, granularity);
    printCandles("BID", product, granularity, bidCandles);
}

void MerkelMain::printCandles(const std::string& type,
                  const std::string& product,
                  const CandlestickGranularity& granularity,
                  const std::vector<Candlestick>& candles) {
    std::cout << type << " " <<"Candlesticks for " << product << " (" << granularity << ")" << std::endl;
    std::cout << "Date, Open, High, Low, Close" << std::endl;
    for (const Candlestick& c : candles) {
        std::cout << c.dateKey << ","
                  << c.open << ","
                  << c.high << ","
                  << c.low << ","
                  << c.close << std::endl;
    }
}

void MerkelMain::loadWallet() {
    wallet = Wallet{};
    walletManager.loadWallet(currentUser.getUsername(), wallet);
}

void MerkelMain::depositMoney()
{
    std::cout << "Deposit money" << std::endl;
    std::cout << "Currency (example: BTC): ";
    std::string currency;
    std::getline(std::cin, currency);

    std::cout << "Amount: ";
    std::string amountStr;
    std::getline(std::cin, amountStr);

    double amount = 0;
    try {
        amount = std::stod(amountStr);
    } catch (...) {
        std::cout << "Invalid amount" << std::endl;
        return;
    }

    if (currency.empty() || amount <= 0) {
        std::cout << "Invalid input" << std::endl;
        return;
    }

    try {
        wallet.insertCurrency(currency, amount);
        walletManager.saveWallet(currentUser.getUsername(), wallet);

        double balanceAfter = wallet.getWalletBalanceForCurrency(currency);
        Transaction t{
            currentUser.getUsername(),
            transactionManager.nowTimestamp(),
            "DEPOSIT",
            currency,
            currency,
            amount,
            balanceAfter
        };
        transactionManager.append(t);

        std::cout << "Deposit successful. New balance (" << currency << "): " << balanceAfter << std::endl;
    } catch (...) {
        std::cout << "Deposit failed" << std::endl;
    }
}

void MerkelMain::withdrawMoney()
{
    std::cout << "Withdraw money" << std::endl;
    std::cout << "Currency (example: BTC): ";
    std::string currency;
    std::getline(std::cin, currency);

    std::cout << "Amount: ";
    std::string amountStr;
    std::getline(std::cin, amountStr);

    double amount = 0;
    try {
        amount = std::stod(amountStr);
    } catch (...) {
        std::cout << "Invalid amount" << std::endl;
        return;
    }

    if (currency.empty() || amount <= 0) {
        std::cout << "Invalid input" << std::endl;
        return;
    }

    try {
        if (!wallet.removeCurrency(currency, amount)) {
            std::cout << "Insufficient funds" << std::endl;
            return;
        }
        walletManager.saveWallet(currentUser.getUsername(), wallet);

        double balanceAfter = wallet.getWalletBalanceForCurrency(currency);
        Transaction t {
            currentUser.getUsername(),
            transactionManager.nowTimestamp(),
            "WITHDRAW",
            currency,
            currency,
            amount,
            balanceAfter
        };
        transactionManager.append(t);

        std::cout << "Withdraw successful. New balance (" << currency << "): " << balanceAfter << std::endl;
    } catch (...) {
        std::cout << "Withdraw failed" << std::endl;
    }
}

void MerkelMain::printRecentTransactions()
{
    std::cout << "Recent transactions" << std::endl;
    std::cout << "Filter by product? (example: ETH/BTC). Press Enter for all: ";

    std::string product;
    std::getline(std::cin, product);

    std::vector<Transaction> transactions;
    if (product.empty()) {
        transactions = transactionManager.lastN(currentUser.getUsername(), 5);
    } else {
        transactions = transactionManager.lastNByProduct(currentUser.getUsername(), product, 5);
    }

    if (transactions.empty()) {
        std::cout << "No transactions found" << std::endl;
        return;
    }

    std::cout << "timestamp, action, product, currency, amount, balanceAfter" << std::endl;
    for (const Transaction& t : transactions) {
        std::cout << t.timestamp << ", "
                  << t.action << ", "
                  << t.product << ", "
                  << t.currency << ", "
                  << t.amount << ", "
                  << t.balanceAfter << std::endl;
    }
}

void MerkelMain::printUserStats()
{
    std::cout << "User stats" << std::endl;

    std::cout << "Filter by product? (example: ETH/BTC). Press Enter for all: ";
    std::string product;
    std::getline(std::cin, product);

    int asks = transactionManager.countActions(currentUser.getUsername(), "ASK", product);
    int bids = transactionManager.countActions(currentUser.getUsername(), "BID", product);

    std::cout << "ASK count: " << asks << std::endl;
    std::cout << "BID count: " << bids << std::endl;

    std::cout << "Timeframe start timestamp (example: 2020/06/01 00:00:00). Press Enter for none: ";
    std::string startTs;
    std::getline(std::cin, startTs);

    std::cout << "Timeframe end timestamp (example: 2020/06/30 23:59:59). Press Enter for none: ";
    std::string endTs;
    std::getline(std::cin, endTs);

    std::cout << "Currency filter for total spent (example: BTC). Press Enter for none: ";
    std::string currency;
    std::getline(std::cin, currency);

    double totalSpent = transactionManager.totalSpentInTimeframe(
        currentUser.getUsername(),
        startTs,
        endTs,
        currency
    );

    std::cout << "Total spent in timeframe: " << totalSpent << std::endl;
}

void MerkelMain::simulateUserTrading()
{
    std::cout << "Simulating user trading activities" << std::endl;

    std::string simulationTimestamp = TradeSimulator::simulateForAllProducts(orderBook, wallet, transactionManager, currentUser.getUsername(), 5);

    walletManager.saveWallet(currentUser.getUsername(), wallet);

    currentTime = simulationTimestamp;

    std::cout << "Simulation done. Current time set to: " << currentTime << std::endl;
}
