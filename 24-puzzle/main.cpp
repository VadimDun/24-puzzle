#include <iostream>
#include <queue>
#include <unordered_set>
#include <map>
#include <fstream>
#include <algorithm>
#include <functional>
#include <chrono>
#include "Heuristic.h"
#include "GameState.h"

using namespace std;
using namespace Heuristics;

static bool check_solvability(const char state[SIZE_OF_FIELD]) {
    int inversions = 0;
    int empty_row = 0;

    for (int i = 0; i < SIZE_OF_FIELD; i++) {
        if (state[i] == '0') {
            empty_row = i / DIMENSITY;
            continue;
        }

        for (int j = i + 1; j < SIZE_OF_FIELD; j++) {
            if (state[j] == '0') continue;

            int val1 = (state[i] >= '1' && state[i] <= '9') ?
                (state[i] - '0') : (state[i] - 'A' + 10);
            int val2 = (state[j] >= '1' && state[j] <= '9') ?
                (state[j] - '0') : (state[j] - 'A' + 10);

            if (val1 > val2) {
                inversions++;
            }
        }
    }
    if (DIMENSITY % 2 == 0)
        return (inversions + empty_row + 1) % 2 == 0;
    else return inversions % 2 == 0;
}

static void charToValues(GameState& gameState) {
    for (int i = 0; i < SIZE_OF_FIELD; ++i) {
        if (gameState.gameState.state[i] == '0') {
            gameState.gameState.state[i] = SIZE_OF_FIELD;
            continue;
        }
        gameState.gameState.state[i] = (gameState.gameState.state[i] >= '1' && gameState.gameState.state[i] <= '9')
            ? (gameState.gameState.state[i] - '0') : (gameState.gameState.state[i] - 'A' + 10);
    }
}

static void charToValues(GameStateArray& gameState) {
    for (int i = 0; i < SIZE_OF_FIELD; ++i) {
        if (gameState.state[i] == '0') {
            gameState.state[i] = SIZE_OF_FIELD;
            continue;
        }
        gameState.state[i] = (gameState.state[i] >= '1' && gameState.state[i] <= '9')
            ? (gameState.state[i] - '0') : (gameState.state[i] - 'A' + 10);
    }
}

static inline int get_emptyPos(const char state[SIZE_OF_FIELD]) {
    int empty_pos = -1;
    for (int i = 0; i < SIZE_OF_FIELD; ++i) {
        if (state[i] == '0') {
            empty_pos = i;
            break;
        }
    }
    return empty_pos;
}

struct Solution {
    CompactMoveStorage moves;

    Solution() = default;

    short getMovesCount() const {
        return moves.getMovesCount();
    }

    void copyToChars(char* dest) const {
        moves.copyToChars(dest);
    }
};

static Solution solveAStar(const char start_state[SIZE_OF_FIELD], int& states_explored, Heuristic heuristic_type) {
    Solution solution;
    uint8_t empty_pos = get_emptyPos(start_state);
    GameStateArray gsa(start_state);
    charToValues(gsa);

    GameState start_game_state(gsa, empty_pos, 0, {}, heuristic_type);

    if (start_game_state.isSolved())
        return solution;

    priority_queue<GameState, vector<GameState>, greater<GameState>> pr_q;
    pr_q.push(start_game_state);

    unordered_set<size_t> visited;
    visited.insert(GameStateArrayHash()(start_state));
    states_explored = 0;

    while (!pr_q.empty()) {
        GameState current = pr_q.top();
        pr_q.pop();
        ++states_explored;

        //if (states_explored % 100000 == 0) cout << states_explored << " " << pr_q.size() << ' ' << current.g_cost << ' ' << current.h_cost << endl;

        uint8_t neighbors_count;
        GameState* neighbors = current.getNextStates(neighbors_count);

        for (int i = 0; i < neighbors_count; ++i) {
            size_t neighbor_hash = GameStateArrayHash()(neighbors[i].gameState);
            if (visited.find(neighbor_hash) != visited.end())
                continue;

            if (neighbors[i].isSolved()) {
                solution.moves.copyFrom(neighbors[i].moves);
                return solution;
            }

            visited.insert(neighbor_hash);
            pr_q.push(neighbors[i]);
        }
    }
    return solution;
}

static void printBoard(const char state[SIZE_OF_FIELD], ofstream& output) {
    for (int i = 0; i < DIMENSITY; ++i) {
        for (int j = 0; j < DIMENSITY; ++j) {
            char c = state[i * DIMENSITY + j];
            if (c == '0') {
                output << " . ";
            }
            else {
                output << " " << c << " ";
            }
        }
        output << endl;
    }
}

static void writeFullSolveInOutput(const char start_state[SIZE_OF_FIELD], const Solution& solution, ofstream& output, const string& algorithm_name) {
    char current_state[SIZE_OF_FIELD];
    memcpy(current_state, start_state, SIZE_OF_FIELD);

    int empty_pos = get_emptyPos(start_state);

    output << "Start:\n";
    printBoard(current_state, output);

    char move_string[GOD_DIGIT];
    solution.copyToChars(move_string);

    for (short i = 0; i < solution.getMovesCount(); ++i) {
        char move = move_string[i];
        int row = empty_pos / DIMENSITY;
        int col = empty_pos % DIMENSITY;
        int new_pos = empty_pos;

        switch (move) {
        case 'U': new_pos = (row - 1) * DIMENSITY + col; break;
        case 'D': new_pos = (row + 1) * DIMENSITY + col; break;
        case 'L': new_pos = row * DIMENSITY + (col - 1); break;
        case 'R': new_pos = row * DIMENSITY + (col + 1); break;
        }

        swap(current_state[empty_pos], current_state[new_pos]);
        empty_pos = new_pos;

        output << (i + 1) << " (" << move << "): ";
        for (int j = 0; j < SIZE_OF_FIELD; ++j) {
            output << current_state[j];
        }
        output << endl;

        printBoard(current_state, output);
    }

    output << "=========================================" << endl << endl;
}

static void result(const char input[SIZE_OF_FIELD], const Solution& solution, const string& algorithm_name,
    ofstream& output, int& states_explored, long long duration) {
    if (solution.getMovesCount() == 0) {
        cout << endl << "Solution isn't found (probably, count limit)" << endl;
        output << "Solution isn't found by " << algorithm_name << endl;
    }
    else {
        char move_string[GOD_DIGIT];
        solution.copyToChars(move_string);

        cout << "; Length: " << solution.getMovesCount() << endl << "Sequence of moves: ";
        for (int i = 0; i < solution.getMovesCount(); ++i) {
            cout << move_string[i];
        }
        cout << endl;

        output << "\nAlgorithm: " << algorithm_name << endl << "Start position: ";
        for_each(input, input + SIZE_OF_FIELD, [&output](char c) { output << c; });
        output << endl << "Length: " << solution.getMovesCount() << " steps" << endl << "Sequence of moves: ";
        for (int i = 0; i < solution.getMovesCount(); ++i) {
            output << move_string[i];
        }
        output << endl;
    }
    output << "Count of states: " << states_explored << endl
        << "Time: " << (double)duration / 1000 << " ms" << endl;
}


struct Algorithm {
    string name;
    function<Solution(const char*, int&)> func;

    Algorithm(string name, function<Solution(const char*, int&)> func) : name(name), func(func) {}
};

Algorithm algorithms[] = {
    Algorithm("A* Manhattan", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::MANHATTAN); }),
    Algorithm("A* linear", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::LINEAR_CONFLICT); }),
    Algorithm("A* corner", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::CORNER_CONFLICTS); }),
    Algorithm("A* corner&linear", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::CORNER_LINEAR_CONFLICTS); }),
};

static Solution execute(Algorithm alg, const char input[SIZE_OF_FIELD], ofstream& output) {
    Solution solution;
    int states_explored = 0;

    auto time = chrono::high_resolution_clock::now();
    solution = alg.func(input, states_explored);
    auto time1 = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(time1 - time).count();

    cout << "Alg: " << alg.name << "; Time: " << (double)duration / 1000 << " ms" << "; \tCount of states: " << states_explored;

    result(input, solution, alg.name, output, states_explored, duration);
    cout << "==============================================" << endl;

    return solution;
}

static void user_input_all_alg() {
    cout << "Write start position: ";
    char input[SIZE_OF_FIELD];
    cin >> input;

    if (strlen(input) != SIZE_OF_FIELD) {
        cout << "Wrong len of start position. It must be " << SIZE_OF_FIELD << endl;
        return;
    }
    transform(begin(input), end(input), begin(input), ::toupper);

    for (char c : input) {
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z'))) {
            cout << "Incorrect char " << c << endl;
            return;
        }
    }

    if (memcmp(input, solved, SIZE_OF_FIELD) == 0) {
        cout << "WOW!" << endl;
        return;
    }

    if (!check_solvability(input)) {
        cout << "This start position is unsolvable" << endl;
        return;
    }

    ofstream output("solution.txt");
    if (!output.is_open()) {
        cout << "solution.txt isn't created" << endl;
        return;
    }

    for (const auto& alg : algorithms) {
        Solution  solution = execute(alg, input, output);
        writeFullSolveInOutput(input, solution, output, alg.name);
    }
    cout << endl << endl;

    output.close();
}

static void many_input_all_alg() {
    ofstream output("solution.txt");
    if (!output.is_open()) {
        cout << "solution.txt isn't created" << endl;
        return;
    }

    vector<const char*> vec;

    switch (DIMENSITY)
    {
    case 4: vec = move(vec4); break;
    case 5: vec = move(vec5); break;
    default: vec = vec4; break;
    }

    for (const char* state_str : vec) {
        for (const auto& alg : algorithms) {
            if (!check_solvability(state_str)) {
                cout << "This start position is unsolvable" << endl;
                return;
            }
            execute(alg, state_str, output);
        }
        cout << endl << endl;
    }
}

int main() {
    setlocale(LC_ALL, "ru");

    //user_input_all_alg();
    many_input_all_alg();

    return 0;
}