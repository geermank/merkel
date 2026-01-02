#include "Transaction.h"

Transaction::Transaction()
    : username(""),
      timestamp(""),
      action(""),
      product(""),
      currency(""),
      amount(0),
      balanceAfter(0) {}

Transaction::Transaction(const std::string& username,
                         const std::string& timestamp,
                         const std::string& action,
                         const std::string& product,
                         const std::string& currency,
                         const double& amount,
                         const double& balanceAfter)
    : username(username),
      timestamp(timestamp),
      action(action),
      product(product),
      currency(currency),
      amount(amount),
      balanceAfter(balanceAfter) {}
