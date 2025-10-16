#pragma once
#include <iostream>

const uint8_t DIMENSITY = 5;
const uint8_t SIZE_OF_FIELD = DIMENSITY * DIMENSITY;

struct GameStateArray {
    char state[SIZE_OF_FIELD];

    GameStateArray(const char st[SIZE_OF_FIELD]);

    GameStateArray() = default;

    bool operator==(const GameStateArray& other) const;
};

struct GameStateArrayHash {
    size_t operator()(const GameStateArray& gsa) const;
};
