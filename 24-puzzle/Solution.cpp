#include "Solution.h"

short Solution::getMovesCount() const {
    return moves.getMovesCount();
}

void Solution::copyToChars(char* dest) const {
    moves.copyToChars(dest);
}