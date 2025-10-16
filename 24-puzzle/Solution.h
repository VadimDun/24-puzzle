#pragma once
#include <functional>
#include <iostream>
#include "GameState.h"

struct Solution {
    CompactMoveStorage moves;

    Solution() = default;

    short getMovesCount() const;

    void copyToChars(char* dest) const;
};

struct Algorithm {
    std::string name;
    std::function<Solution(const char*, int&)> func;

    Algorithm(std::string name, std::function<Solution(const char*, int&)> func) : name(name), func(func) {}
};