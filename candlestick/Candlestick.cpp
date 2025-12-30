#include "Candlestick.h"

Candlestick::Candlestick()
    : product(""), type(""), dateKey(""), open(0), high(0), low(0), close(0) {}

Candlestick::Candlestick(const std::string& product,
                         const std::string& type,
                         const std::string& dateKey,
                         const double& open,
                         const double& high,
                         const double& low,
                         const double& close)
    : product(product),
      type(type),
      dateKey(dateKey),
      open(open),
      high(high),
      low(low),
      close(close) {}
