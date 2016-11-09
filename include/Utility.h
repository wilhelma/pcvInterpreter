#ifndef UTILITY_H_
#define UTILITY_H_

#include "Types.h"

#include <chrono>
#include <random>
#include <string>
#include <iostream>

static inline std::string random_string(size_t length)
{
    static const std::string alphanums =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static std::mt19937 rg{std::random_device{}()};
    static std::uniform_int_distribution<> pick(0, alphanums.size() - 1);

    std::string s;

    s.reserve(length);

    while(length--)
        s += alphanums[pick(rg)];

    return s;
}

#endif /* UTILITY_H_ */