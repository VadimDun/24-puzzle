#include "GameStateArray.h"


GameStateArray::GameStateArray(const char st[SIZE_OF_FIELD]) {
    memcpy(state, st, SIZE_OF_FIELD);
}

bool GameStateArray::operator==(const GameStateArray& other) const {
    return memcmp(state, other.state, SIZE_OF_FIELD) == 0;
}


size_t GameStateArrayHash::operator()(const GameStateArray& gsa) const {
    size_t hash = 0;
    for (int i = 0; i < SIZE_OF_FIELD; ++i) {
        hash = hash * 31 + gsa.state[i];
    }
    return hash;
}
