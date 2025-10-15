#pragma once
#include <iostream>
#include <vector>
#include "GameStateArray.h"

namespace Heuristics {

    enum class Heuristic : uint8_t {
        MANHATTAN,
        LINEAR_CONFLICT,
        CORNER_CONFLICTS,
        CORNER_LINEAR_CONFLICTS,
    };


    static short calculateManhattanDistance(const GameStateArray& gsa, uint8_t empty_pos);

    static short calculateLinearConflict(const GameStateArray& gsa, uint8_t empty_pos);

    static uint8_t calculateLinearConflicts(const GameStateArray& gsa, uint8_t empty_pos);

    static short calculateCornerConflict(const GameStateArray& gsa, uint8_t empty_pos);

    static short calculateCornerLinearConflict(const GameStateArray& gsa, uint8_t empty_pos);

    static short calculateCornerConflicts(const GameStateArray& gsa);

    short calculateHeuristic(const GameStateArray& gsa, Heuristic heuristic_type);

    static short calculateManhattanDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateManhattanDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateLinearDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateCornerDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateLinearConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateCornerConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    static short calculateCornerLinearConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos);

    short calculateHeuristicDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos, Heuristic heuristic_type);
}