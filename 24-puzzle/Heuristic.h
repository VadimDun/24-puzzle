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

    short calculateHeuristic(const GameStateArray& gsa, Heuristic heuristic_type);

    char calculateHeuristicDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos, Heuristic heuristic_type);
}