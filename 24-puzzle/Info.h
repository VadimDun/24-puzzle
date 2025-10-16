#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GameState.h"
#include "Solution.h"

void writeFullSolveInOutput(const char start_state[SIZE_OF_FIELD], const Solution& solution, std::ofstream& output, const std::string& algorithm_name);

void result(const char input[SIZE_OF_FIELD], const Solution& solution, const std::string& algorithm_name,
    std::ofstream& output, int& states_explored, long long duration);