#pragma once
#include <string>

class Candlestick {
public:
    Candlestick();

    Candlestick(const std::string& product,
                const std::string& type,
                const std::string& dateKey,
                const double& open,
                const double& high,
                const double& low,
                const double& close);

    std::string product;
    std::string type;
    std::string dateKey;
    double open;
    double high;
    double low;
    double close;
};
