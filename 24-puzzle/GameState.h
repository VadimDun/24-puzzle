#pragma once
#include <iostream>
#include <vector>
#include "Heuristic.h"
#include "GameStateArray.h"

using namespace Heuristics;

static constexpr uint8_t calculateGodDigit(uint8_t dimensity) {
    uint8_t res = 0;
    switch (dimensity)
    {
    case 3: res = 31; break;
    case 4: res = 80; break;
    case 5: res = 210; break;
    default:
        break;
    }
    return res;
}

const uint8_t GOD_DIGIT = calculateGodDigit(DIMENSITY);
const char solved[37] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

class CompactMoveStorage {
private:
    static constexpr uint8_t BITS_PER_MOVE = 2;                                                  // 2 бита = 4 варианта хода
    static constexpr uint8_t MOVES_PER_CHAR = 8 / BITS_PER_MOVE;                                 // 4 хода на байт
    static constexpr uint8_t BUFFER_SIZE = (GOD_DIGIT + MOVES_PER_CHAR - 1) / MOVES_PER_CHAR;    // кол-во байт

    char buffer[BUFFER_SIZE] = { 0 };
    uint8_t moves_count = 0;

public:
    CompactMoveStorage() = default;

    // Объявления методов
    void addMove(uint8_t move);
    uint8_t getMove(uint8_t index) const;
    char getMoveChar(uint8_t index) const;
    void copyFrom(const CompactMoveStorage& other);
    void copyToChars(char* dest) const;
    uint8_t getMovesCount() const;
};

class GameState {
public:
    GameStateArray gameState;
    uint8_t empty_pos;
    short g_cost;
    short h_cost;
    CompactMoveStorage moves;
    Heuristic heuristic_type;

    GameState(GameStateArray st, uint8_t empty, short g = 0,
        const CompactMoveStorage& m = CompactMoveStorage(),
        Heuristic h = Heuristic::MANHATTAN);
    GameState(const GameState& parent, uint8_t new_empty_pos, const CompactMoveStorage& m, Heuristic h = Heuristic::MANHATTAN);

    GameState() = default;

    // Объявления методов
    short getFCost() const;
    bool isSolved() const;
    GameState* getNextStates(uint8_t& count) const;
    short getMovesCount() const;
    bool operator>(const GameState& other) const;
    bool operator==(const GameState& other) const;

    struct Hash {
        size_t operator()(const GameState& gs) const;
    };
};

// Объявления внешних переменных и функций
extern std::vector<const char*> vec4;
extern std::vector<const char*> vec5;
