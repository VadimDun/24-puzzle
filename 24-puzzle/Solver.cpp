#include "Solver.h"

using namespace std;

bool check_solvability(const char state[SIZE_OF_FIELD]) {
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

Solution execute(Algorithm alg, const char input[SIZE_OF_FIELD], ofstream& output) {
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

inline int get_emptyPos(const char state[SIZE_OF_FIELD]) {
    int empty_pos = -1;
    for (int i = 0; i < SIZE_OF_FIELD; ++i) {
        if (state[i] == '0') {
            empty_pos = i;
            break;
        }
    }
    return empty_pos;
}

Solution solveAStar(const char start_state[SIZE_OF_FIELD], int& states_explored, Heuristic heuristic_type) {
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
static bool idaStarSearch(GameState& state, int threshold, int& min_exceed,
    int& states_explored, Solution& solution,
    unordered_set<GameStateArray, GameStateArrayHash>& visited
) {

    states_explored++;
    //if (states_explored % 10000 == 0) {
    //    cout << "IDA* states: " << states_explored << " threshold: " << threshold << " g: " << state.getMovesCount() << endl;
    //}

    int f_cost = state.getFCost();

    if (f_cost > threshold) {
        if (f_cost < min_exceed) {
            min_exceed = f_cost;
        }
        return false;
    }

    if (state.isSolved()) {
        solution.moves.copyFrom(state.moves);
        return true;
    }

    uint8_t neighbors_count = 0;
    GameState* neighbors = state.getNextStates(neighbors_count);

    vector<pair<int, GameState>> sorted_neighbors;
    for (int i = 0; i < neighbors_count; i++) {
        int neighbor_h = neighbors[i].getFCost();
        sorted_neighbors.push_back(make_pair(neighbor_h, neighbors[i]));
    }
    
    sort(sorted_neighbors.begin(), sorted_neighbors.end(),
        [](const pair<int, GameState>& a, const pair<int, GameState>& b) {
            return a.first < b.first;
        });
    
    for (size_t i = 0; i < sorted_neighbors.size(); i++) {
        GameState& neighbor = sorted_neighbors[i].second;
        size_t neighbor_hash = GameStateArrayHash()(neighbor.gameState);
        if (visited.find(neighbor.gameState) != visited.end())
            continue;
        visited.insert(neighbor.gameState);

        if (idaStarSearch(neighbor, threshold, min_exceed, states_explored, solution, visited)) {
            return true;
        }
        visited.erase(neighbor.gameState);
    }

    return false;
}

Solution solveIDAStar(const char start_state[SIZE_OF_FIELD], int& states_explored, Heuristic heuristic_type) {
    Solution solution;

    states_explored = 0;
    uint8_t empty_pos = get_emptyPos(start_state);
    GameStateArray gsa(start_state);
    charToValues(gsa);

    GameState start_game_state(gsa, empty_pos, 0, {}, heuristic_type);

    if (start_game_state.isSolved()) {
        return solution;
    }

    int threshold = start_game_state.getFCost();
    const int MAX_THRESHOLD = GOD_DIGIT * 2; // Увеличиваем максимальный порог

    while (threshold <= GOD_DIGIT) {
        int min_exceed = INT_MAX;

        unordered_set<GameStateArray, GameStateArrayHash> visited;
        visited.insert(start_game_state.gameState);

        bool found = idaStarSearch(start_game_state, threshold, min_exceed, states_explored, solution, visited);

        if (found) {
            return solution;
        }

        if (min_exceed == INT_MAX) {
            break;
        }

        threshold = min_exceed;
    }

    return solution;
}
