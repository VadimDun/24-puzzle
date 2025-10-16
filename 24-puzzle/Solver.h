#pragma once
#include <queue>
#include <unordered_set>
#include <map>
#include <vector>
#include <chrono>
#include "Heuristic.h"
#include "Solution.h"
#include "GameState.h"
#include "Info.h"

inline int get_emptyPos(const char state[SIZE_OF_FIELD]);

bool check_solvability(const char state[SIZE_OF_FIELD]);

Solution solveAStar(const char start_state[SIZE_OF_FIELD], int& states_explored, Heuristic heuristic_type);

Solution execute(Algorithm alg, const char input[SIZE_OF_FIELD], std::ofstream& output);