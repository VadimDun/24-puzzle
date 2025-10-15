#include "GameState.h"
#include <cstring>

using namespace std;
using namespace Heuristics;


// A=10, B=11, C=12, D=13, E=14, F=15, G=16, H=17, I=18, J=19, 
// K=20, L=21, M=22, N=23, O=24, P=25, Q=26, R=27, S=28, T=29, 
// U=30, V=31, W=32, X=33, Y=34, Z=35

vector<const char*> vec4 = {
    //"1234067859ACDEBF", // 5
    //"7023168459ACDEBF", // 15
    //"7283160459ACDEBF", // 17
    //"7283106459ACDEBF", // 18
    "12345678A0BE9FCD", // 19 
    "F2345678A0BE91DC", // 33
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

///////////////////////////// CompactMoveStorage /////////////////////////////

// Добавление хода (0-3 соответствует U,R,D,L)
void CompactMoveStorage::addMove(uint8_t move) {
    if (moves_count >= GOD_DIGIT) return;

    uint8_t char_index = moves_count / MOVES_PER_CHAR;
    uint8_t bit_offset = (moves_count % MOVES_PER_CHAR) * BITS_PER_MOVE;

    buffer[char_index] &= ~(0x03 << bit_offset);        // очищаем 2 бита: 0x03 в двоичном виде = 00000011.
    // 0x03 << 6 = 00000011 << 6 = 11000000
    // ~(0x03 << 6) = ~11000000 = 00111111
    buffer[char_index] |= (move & 0x03) << bit_offset;  // записываем новое значение

    ++moves_count;
}

uint8_t CompactMoveStorage::getMove(uint8_t index) const {
    if (index >= moves_count) return 0;

    uint8_t char_index = index / MOVES_PER_CHAR;
    uint8_t bit_offset = (index % MOVES_PER_CHAR) * BITS_PER_MOVE;

    return (buffer[char_index] >> bit_offset) & 0x03; //(11000110 >> 6) = 00000011. (00000011 & 0x03) = (00000011 & 00000011) = 00000011 = 3
}

char CompactMoveStorage::getMoveChar(uint8_t index) const {
    uint8_t move = getMove(index);
    return moveChars[move];
}

void CompactMoveStorage::copyFrom(const CompactMoveStorage& other) {
    moves_count = other.moves_count;
    memcpy(buffer, other.buffer, (moves_count + MOVES_PER_CHAR - 1) / MOVES_PER_CHAR);
}

void CompactMoveStorage::copyToChars(char* dest) const {
    for (uint8_t i = 0; i < moves_count; ++i) {
        dest[i] = getMoveChar(i);
    }
}

uint8_t CompactMoveStorage::getMovesCount() const { return moves_count; }

///////////////////////////// GameState /////////////////////////////

GameState::GameState(GameStateArray st, uint8_t empty, short g,
    const CompactMoveStorage& m, Heuristic h)
    : gameState(st), empty_pos(empty), g_cost(g), heuristic_type(h)
{
    moves.copyFrom(m);
    h_cost = calculateHeuristic(st, h);
}

GameState::GameState(const GameState& parent, uint8_t new_empty_pos, char move_dir, Heuristic h)
    : gameState(parent.gameState), empty_pos(new_empty_pos), g_cost(parent.g_cost + 1),
    heuristic_type(h), moves(parent.moves)
{
    std::swap(gameState.state[parent.empty_pos], gameState.state[new_empty_pos]);

    h_cost = parent.h_cost + calculateHeuristicDelta(gameState, new_empty_pos, parent.empty_pos, h);
    moves.addMove(move_dir);
}

short GameState::getFCost() const {
    return g_cost + h_cost;
}

bool GameState::isSolved() const {
    return h_cost == 0;
}

GameState* GameState::getNextStates(uint8_t& count) const {
    static GameState nextStates[4];
    count = 0;
    uint8_t row = empty_pos / DIMENSITY;
    uint8_t col = empty_pos % DIMENSITY;

    for (uint8_t i = 0; i < 4; ++i) {
        uint8_t new_row = row + directions[i][0];
        uint8_t new_col = col + directions[i][1];

        if (new_row < DIMENSITY && new_col < DIMENSITY) {
            uint8_t new_pos = new_row * DIMENSITY + new_col;

            //GameStateArray new_state_array(gameState.state);
            //std::swap(new_state_array.state[empty_pos], new_state_array.state[new_pos]);

            //nextStates[count] = GameState(new_state_array, new_pos, g_cost + 1, moves, heuristic_type);
            //nextStates[count].moves.addMove(i);
            nextStates[count] = GameState(*this, new_pos, moveChars[i], heuristic_type);
            ++count;
        }
    }
    return nextStates;
}

short GameState::getMovesCount() const {
    return moves.getMovesCount();
}

bool GameState::operator>(const GameState& other) const {
    return getFCost() > other.getFCost();
}

bool GameState::operator==(const GameState& other) const {
    return gameState == other.gameState;
}

size_t GameState::Hash::operator()(const GameState& gs) const {
    return GameStateArrayHash()(gs.gameState.state);
}

///////////////////////////// Other functions /////////////////////////////

