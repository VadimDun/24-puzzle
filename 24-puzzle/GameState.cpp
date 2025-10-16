#include "GameState.h"
#include <cstring>

using namespace std;
using namespace Heuristics;

const int directions[4][2] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
const char moveChars[4] = { 'U', 'R', 'D', 'L' };

///////////////////////////// CompactMoveStorage /////////////////////////////

// Добавление хода (0-3 соответствует U,R,D,L)
void CompactMoveStorage::addMove(uint8_t move) {
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
    prev_dir = -1;
    moves.copyFrom(m);
    h_cost = calculateHeuristic(st, h);
}

GameState::GameState(const GameState& parent, uint8_t new_empty_pos, int8_t pr_dir, const CompactMoveStorage& m, Heuristic h)
    : gameState(parent.gameState), empty_pos(new_empty_pos), prev_dir(pr_dir), g_cost(parent.g_cost + 1), heuristic_type(h)
{
    std::swap(gameState.state[parent.empty_pos], gameState.state[new_empty_pos]);
    h_cost = parent.h_cost + calculateHeuristicDelta(gameState, new_empty_pos, parent.empty_pos, h);
    moves.copyFrom(m);
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
        if ((i + 2) % 4 == prev_dir) continue;

        uint8_t new_row = row + directions[i][0];
        uint8_t new_col = col + directions[i][1];

        if (new_row < DIMENSITY && new_col < DIMENSITY) {
            uint8_t new_pos = new_row * DIMENSITY + new_col;

            //GameStateArray new_state_array(gameState.state);
            //std::swap(new_state_array.state[empty_pos], new_state_array.state[new_pos]);

            //nextStates[count] = GameState(new_state_array, new_pos, g_cost + 1, moves, heuristic_type);
            //nextStates[count].moves.addMove(i);

            CompactMoveStorage m;
            m.copyFrom(moves);
            m.addMove(i);
            nextStates[count] = GameState(*this, new_pos, i, m, heuristic_type);
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

