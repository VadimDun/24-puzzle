#include <iostream>
#include <queue>
#include <unordered_set>
#include <map>
#include <fstream>
#include <algorithm>
#include <functional>
#include <chrono>

using namespace std;

static constexpr uint16_t calculateGodDigit(uint8_t dimensity) {
    uint16_t res = 0;
    switch (dimensity)
    {
    case 3: res = 31; break;
    case 4: res = 80; break;
    case 5: res = 210; break;
    case 6: res = 358; break;
    default:
        break;
    }
    return res;
}

const uint8_t DIMENSITY = 4;
const uint8_t SIZE_OF_FIELD = DIMENSITY * DIMENSITY;
const uint16_t GOD_DIGIT = calculateGodDigit(DIMENSITY);
char solved[SIZE_OF_FIELD];

const uint8_t directions[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
const char moveChars[4] = { 'U', 'R', 'D', 'L' };

// A=10, B=11, C=12, D=13, E=14, F=15, G=16, H=17, I=18, J=19, 
// K=20, L=21, M=22, N=23, O=24, P=25, Q=26, R=27, S=28, T=29, 
// U=30, V=31, W=32, X=33, Y=34, Z=35

vector<const char*> vec4 = {
    //"1234067859ACDEBF", // 5
    //"7023168459ACDEBF", // 15
    //"7283160459ACDEBF", // 17
    //"7283106459ACDEBF", // 18
    //"12345678A0BE9FCD", // 19 
    //"F2345678A0BE91DC", // 33
    "75AB2C416D389F0E", // 45
    "FE169B4C0A73D852", // 52
    "D79F2E8A45106C3B", // 55
    //"BAC0F478E19623D5", // 61
};

vector<const char*> vec5 = {
    //"123456789ABCDEFGHIJ0LMNOK", // 1
    "123456789ABCDEFGHIJKLMN0O", // 1
    //"F63M09LE51BO27IDNJACH48KG", // 1
    "D7IMLOB2H18N4035CG9KFJ6EA", // 69
    //"K12HACBO6G9E4L63JM07FDNI5", //
    //"DGC8A25HIJLM0ENB67F19KO43", //
    //"JOHIF9AL8B2E063G7M5K4CD1N", //
};

enum class Heuristic : uint8_t {
    MANHATTAN,
    LINEAR_CONFLICT,
    CORNER_CONFLICTS,
    CORNER_LINEAR_CONFLICTS,
};

struct GameStateArray {
    char state[SIZE_OF_FIELD];

    GameStateArray(const char st[SIZE_OF_FIELD]) {
        memcpy(state, st, SIZE_OF_FIELD);
    }

    GameStateArray() = default;


    bool operator==(const GameStateArray& other) const {
        return memcmp(state, other.state, SIZE_OF_FIELD) == 0;
    }
};

struct GameStateArrayHash {
    size_t operator()(const GameStateArray& gsa) const {
        size_t hash = 0;
        for (int i = 0; i < SIZE_OF_FIELD; ++i) {
            hash = hash * 31 + gsa.state[i];
        }
        return hash;
    }
};

class CompactMoveStorage {
private:
    static constexpr uint8_t BITS_PER_MOVE = 2;                                                 // 2 бита = 4 варианта хода
    static constexpr uint8_t MOVES_PER_CHAR = 8 / BITS_PER_MOVE;                                // 4 хода на байт
    static constexpr uint16_t BUFFER_SIZE = (GOD_DIGIT + MOVES_PER_CHAR - 1) / MOVES_PER_CHAR;  // кол-во байт

    char buffer[BUFFER_SIZE] = { 0 };
    uint16_t moves_count = 0;
public:
    CompactMoveStorage() = default;

    // Добавление хода (0-3 соответствует U,R,D,L)
    void addMove(uint8_t move) {
        if (moves_count >= GOD_DIGIT) return;

        uint8_t char_index = moves_count / MOVES_PER_CHAR;
        uint8_t bit_offset = (moves_count % MOVES_PER_CHAR) * BITS_PER_MOVE;

        buffer[char_index] &= ~(0x03 << bit_offset);        // очищаем 2 бита: 0x03 в двоичном виде = 00000011.
                                                            // 0x03 << 6 = 00000011 << 6 = 11000000
                                                            // ~(0x03 << 6) = ~11000000 = 00111111
        buffer[char_index] |= (move & 0x03) << bit_offset;  // записываем новое значение

        ++moves_count;
    }

    uint8_t getMove(uint8_t index) const {
        if (index >= moves_count) return 0;

        uint8_t char_index = index / MOVES_PER_CHAR;
        uint8_t bit_offset = (index % MOVES_PER_CHAR) * BITS_PER_MOVE;

        return (buffer[char_index] >> bit_offset) & 0x03; //(11000110 >> 6) = 00000011. (00000011 & 0x03) = (00000011 & 00000011) = 00000011 = 3
    }

    char getMoveChar(uint8_t index) const {
        uint8_t move = getMove(index);
        return moveChars[move];
    }

    void copyFrom(const CompactMoveStorage& other, uint16_t count) {
        moves_count = count;
        memcpy(buffer, other.buffer, (count + MOVES_PER_CHAR - 1) / MOVES_PER_CHAR);
    }

    void copyToChars(char* dest) const {
        for (size_t i = 0; i < moves_count; ++i) {
            dest[i] = getMoveChar(i);
        }
    }

    size_t getMovesCount() const { return moves_count; }
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
        const CompactMoveStorage& m = {}, Heuristic h = Heuristic::MANHATTAN)
        : gameState(st), empty_pos(empty), g_cost(g), heuristic_type(h)
    {
        moves.copyFrom(m, m.getMovesCount());
        h_cost = calculateHeuristic();
    }

    GameState() = default;

    short calculateHeuristic() const {
        switch (heuristic_type) {
        case Heuristic::MANHATTAN:
            return calculateManhattanDistance();
        case Heuristic::LINEAR_CONFLICT:
            return calculateLinearConflict();
        case Heuristic::CORNER_CONFLICTS:
            return calculateCornerConflict();
        case Heuristic::CORNER_LINEAR_CONFLICTS:
            return calculateCornerLinearConflict();
        default:
            return calculateManhattanDistance();
        }
    }

    short calculateManhattanDistance() const {
        short distance = 0;
        for (short i = 0; i < SIZE_OF_FIELD; ++i) {
            if (i == empty_pos) continue;

            uint8_t target_row = (gameState.state[i] - 1) / DIMENSITY;
            uint8_t target_col = (gameState.state[i] - 1) % DIMENSITY;
            uint8_t current_row = i / DIMENSITY;
            uint8_t current_col = i % DIMENSITY;

            distance += abs(target_row - current_row) + abs(target_col - current_col);
        }
        return distance;
    }

    short calculateLinearConflict() const {
        return calculateManhattanDistance() + calculateLinearConflicts();
    }

    uint8_t calculateLinearConflicts() const {
        uint8_t conflicts = 0;

        uint8_t tiles[DIMENSITY]{};

        for (uint8_t row = 0; row < DIMENSITY; ++row) {
            uint8_t count = 0;

            for (uint8_t col = 0; col < DIMENSITY; ++col) {
                uint8_t pos = row * DIMENSITY + col;
                if (pos == empty_pos) continue;

                uint8_t tile = gameState.state[pos];
                uint8_t target_row = (tile - 1) / DIMENSITY;

                if (target_row == row) {
                    tiles[count++] = (tile - 1) % DIMENSITY;
                }
            }

            for (uint8_t i = 0; i < count; ++i) {
                for (uint8_t j = i + 1; j < count; ++j) {
                    if (tiles[i] > tiles[j]) {
                        ++conflicts;
                    }
                }
            }
        }

        for (uint8_t col = 0; col < DIMENSITY; ++col) {
            uint8_t count = 0;

            for (uint8_t row = 0; row < DIMENSITY; ++row) {
                uint8_t pos = row * DIMENSITY + col;
                if (pos == empty_pos) continue;

                uint8_t tile = gameState.state[pos];
                uint8_t target_col = (tile - 1) % DIMENSITY;

                if (target_col == col) {
                    tiles[count++] = (tile - 1) / DIMENSITY;
                }
            }

            for (uint8_t i = 0; i < count; ++i) {
                for (uint8_t j = i + 1; j < count; ++j) {
                    if (tiles[i] > tiles[j]) {
                        ++conflicts;
                    }
                }
            }
        }

        return conflicts << 1; // *2
    }

    short calculateCornerConflict() const {
        return calculateManhattanDistance() + calculateCornerConflicts();
    }

    short calculateCornerLinearConflict() const {
        return calculateLinearConflict() + calculateCornerConflicts();
    }

    short calculateCornerConflicts() const {
        uint8_t res = 0;
        uint8_t corner1 = 0;
        uint8_t corner1R = 1;
        uint8_t corner1D = DIMENSITY;
        uint8_t corner2 = DIMENSITY - 1;
        uint8_t corner2L = DIMENSITY - 2;
        uint8_t corner2D = DIMENSITY << 1 - 1;
        uint8_t corner3 = DIMENSITY * (DIMENSITY - 1);
        uint8_t corner3R = DIMENSITY * (DIMENSITY - 1) + 1;
        uint8_t corner3U = DIMENSITY * (DIMENSITY - 2);

        // Значение в клетке должно быть на 1 больше индекса
        //if (!(gameState.state[corner1] == SIZE_OF_FIELD || gameState.state[corner1] == 1)) {
        if (!(gameState.state[corner1D] == SIZE_OF_FIELD || gameState.state[corner1] == SIZE_OF_FIELD || gameState.state[corner1R] == SIZE_OF_FIELD || gameState.state[corner1] == 1)) {
            if (gameState.state[corner1R] == 2) {
                if (gameState.state[corner1D] == corner1D + 1)
                    res += 2;
                else ++res;
            }
            else if (gameState.state[corner1D] == corner1D + 1)
                ++res;
        }

        //if (!(gameState.state[corner2] == SIZE_OF_FIELD || gameState.state[corner2] == DIMENSITY)) {
        if (!(gameState.state[corner2L] == SIZE_OF_FIELD || gameState.state[corner2] == SIZE_OF_FIELD || gameState.state[corner2D] == SIZE_OF_FIELD || gameState.state[corner2] == DIMENSITY)) {
            if (gameState.state[corner2L] == corner2) {
                if (gameState.state[corner2D] == DIMENSITY << 1)
                    res += 2;
                else ++res;
            }
            else if (gameState.state[corner2D] == DIMENSITY << 1)
                ++res;
        }
        //if (!(gameState.state[corner3] == SIZE_OF_FIELD || gameState.state[corner3] == corner3R)) {
        if (!(gameState.state[corner3U] == SIZE_OF_FIELD || gameState.state[corner3] == SIZE_OF_FIELD || gameState.state[corner3R] == SIZE_OF_FIELD || gameState.state[corner3] == corner3R)) {
            if (gameState.state[corner3U] == corner3U + 1) {
                if (gameState.state[corner3R] == corner3R + 1)
                    res += 2;
                else ++res;
            }
            else if (gameState.state[corner3R] == corner3R + 1)
                ++res;
        }
        return res << 1; // *2
    }

    short getFCost() const {
        return g_cost + h_cost;
    }

    bool isSolved() const {
        return memcmp(gameState.state, solved, SIZE_OF_FIELD) == 0;
    }

    GameState* getNextStates(uint8_t& count) const {
        static GameState nextStates[4];
        count = 0;
        uint8_t row = empty_pos / DIMENSITY;
        uint8_t col = empty_pos % DIMENSITY;

        for (uint8_t i = 0; i < 4; ++i) {
            uint8_t new_row = row + directions[i][0];
            uint8_t new_col = col + directions[i][1];

            if (new_row >= 0 && new_row < DIMENSITY && new_col >= 0 && new_col < DIMENSITY) {
                uint8_t new_pos = new_row * DIMENSITY + new_col;

                GameStateArray new_state_array(gameState.state);
                std::swap(new_state_array.state[empty_pos], new_state_array.state[new_pos]);

                nextStates[count] = GameState(new_state_array, new_pos, g_cost + 1, moves, heuristic_type);
                nextStates[count].moves.addMove(i);
                ++count;
            }
        }
        return nextStates;
    }

    short getMovesCount() const {
        return moves.getMovesCount();
    }

    bool operator>(const GameState& other) const {
        return getFCost() > other.getFCost();
    }

    bool operator==(const GameState& other) const {
        return gameState == other.gameState;
    }

    struct Hash {
        size_t operator()(const GameState& gs) const {
            return GameStateArrayHash()(gs.gameState.state);
        }
    };
};

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

    GameState start_game_state(start_state, empty_pos, 0, {}, heuristic_type);
    charToValues(start_game_state);

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
                solution.moves.copyFrom(neighbors[i].moves, neighbors[i].getMovesCount());
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

    for (short i = 0; i < SIZE_OF_FIELD; ++i)
        solved[i] = i + 1;

    //user_input_all_alg();
    many_input_all_alg();

    return 0;
}