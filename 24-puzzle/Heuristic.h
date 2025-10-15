#pragma once
#include <iostream>
#include <vector>
#include "GameStateArray.h"

enum class Heuristic : uint8_t {
    MANHATTAN,
    LINEAR_CONFLICT,
    CORNER_CONFLICTS,
    CORNER_LINEAR_CONFLICTS,
};


short calculateManhattanDistance(GameStateArray& gsa, uint8_t empty_pos);

short calculateLinearConflict(GameStateArray& gsa, uint8_t empty_pos);

uint8_t calculateLinearConflicts(GameStateArray& gsa, uint8_t empty_pos);

short calculateCornerConflict(GameStateArray& gsa, uint8_t empty_pos);

short calculateCornerLinearConflict(GameStateArray& gsa, uint8_t empty_pos);

short calculateCornerConflicts(GameStateArray& gsa);

short calculateHeuristic(GameStateArray& gsa, Heuristic heuristic_type);

