#include "Heuristic.h"

namespace Heuristics {

    static short calculateManhattanDistance(const GameStateArray& gsa, uint8_t empty_pos) {
        short distance = 0;
        for (short i = 0; i < SIZE_OF_FIELD; ++i) {
            if (i == empty_pos) continue;

            uint8_t target_row = (gsa.state[i] - 1) / DIMENSITY;
            uint8_t target_col = (gsa.state[i] - 1) % DIMENSITY;
            uint8_t current_row = i / DIMENSITY;
            uint8_t current_col = i % DIMENSITY;

            distance += abs(target_row - current_row) + abs(target_col - current_col);
        }
        return distance;
    }

    static short calculateLinearConflict(const GameStateArray& gsa, uint8_t empty_pos) {
        return calculateManhattanDistance(gsa, empty_pos) + calculateLinearConflicts(gsa, empty_pos);
    }

    static uint8_t calculateLinearConflicts(const GameStateArray& gsa, uint8_t empty_pos) {
        uint8_t conflicts = 0;

        uint8_t tiles[DIMENSITY]{};

        for (uint8_t row = 0; row < DIMENSITY; ++row) {
            uint8_t count = 0;

            for (uint8_t col = 0; col < DIMENSITY; ++col) {
                uint8_t pos = row * DIMENSITY + col;
                if (pos == empty_pos) continue;

                uint8_t tile = gsa.state[pos];
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

                uint8_t tile = gsa.state[pos];
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

    static short calculateCornerConflict(const GameStateArray& gsa, uint8_t empty_pos) {
        return calculateManhattanDistance(gsa, empty_pos) + calculateCornerConflicts(gsa);
    }

    static short calculateCornerLinearConflict(const GameStateArray& gsa, uint8_t empty_pos) {
        return calculateLinearConflict(gsa, empty_pos) + calculateCornerConflicts(gsa);
    }

    static short calculateCornerConflicts(const GameStateArray& gsa) {
        uint8_t res = 0;
        uint8_t corner1 = 0;
        uint8_t corner1R = 1;
        uint8_t corner1D = DIMENSITY;
        uint8_t corner2 = DIMENSITY - 1;
        uint8_t corner2L = DIMENSITY - 2;
        uint8_t corner2D = (DIMENSITY << 1) - 1;
        uint8_t corner3 = DIMENSITY * (DIMENSITY - 1);
        uint8_t corner3R = DIMENSITY * (DIMENSITY - 1) + 1;
        uint8_t corner3U = DIMENSITY * (DIMENSITY - 2);

        // «начение в клетке должно быть на 1 больше индекса
        //if (!(gsa.state[corner1] == SIZE_OF_FIELD || gsa.state[corner1] == 1)) {
        if (!(gsa.state[corner1D] == SIZE_OF_FIELD || gsa.state[corner1] == SIZE_OF_FIELD || gsa.state[corner1R] == SIZE_OF_FIELD || gsa.state[corner1] == 1)) {
            if (gsa.state[corner1R] == 2) {
                if (gsa.state[corner1D] == corner1D + 1)
                    res += 2;
                else ++res;
            }
            else if (gsa.state[corner1D] == corner1D + 1)
                ++res;
        }

        //if (!(gsa.state[corner2] == SIZE_OF_FIELD || gsa.state[corner2] == DIMENSITY)) {
        if (!(gsa.state[corner2L] == SIZE_OF_FIELD || gsa.state[corner2] == SIZE_OF_FIELD || gsa.state[corner2D] == SIZE_OF_FIELD || gsa.state[corner2] == DIMENSITY)) {
            if (gsa.state[corner2L] == corner2) {
                if (gsa.state[corner2D] == DIMENSITY << 1)
                    res += 2;
                else ++res;
            }
            else if (gsa.state[corner2D] == DIMENSITY << 1)
                ++res;
        }
        //if (!(gsa.state[corner3] == SIZE_OF_FIELD || gsa.state[corner3] == corner3R)) {
        if (!(gsa.state[corner3U] == SIZE_OF_FIELD || gsa.state[corner3] == SIZE_OF_FIELD || gsa.state[corner3R] == SIZE_OF_FIELD || gsa.state[corner3] == corner3R)) {
            if (gsa.state[corner3U] == corner3U + 1) {
                if (gsa.state[corner3R] == corner3R + 1)
                    res += 2;
                else ++res;
            }
            else if (gsa.state[corner3R] == corner3R + 1)
                ++res;
        }
        return res << 1; // *2
    }

    static inline uint8_t get_emptyPos(const GameStateArray& gsa) {
        uint8_t empty_pos = 255;
        for (uint8_t i = 0; i < SIZE_OF_FIELD; ++i) {
            if (gsa.state[i] == SIZE_OF_FIELD) {
                empty_pos = i;
                break;
            }
        }
        return empty_pos;
    }

    short calculateHeuristic(const GameStateArray& gsa, Heuristic heuristic_type) {
        uint8_t empty_pos = get_emptyPos(gsa);
        switch (heuristic_type) {
        case Heuristic::MANHATTAN:
            return calculateManhattanDistance(gsa, empty_pos);
        case Heuristic::LINEAR_CONFLICT:
            return calculateLinearConflict(gsa, empty_pos);
        case Heuristic::CORNER_CONFLICTS:
            return calculateCornerConflict(gsa, empty_pos);
        case Heuristic::CORNER_LINEAR_CONFLICTS:
            return calculateCornerLinearConflict(gsa, empty_pos);
        default:
            return calculateManhattanDistance(gsa, empty_pos);
        }
    }

    static short calculateManhattanDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) {
        uint8_t moved_tile = gsa.state[old_empty_pos]; // ѕлитка переместилась на место пустой
    
        // ¬ычисл€ем целевую позицию перемещенной плитки
        uint8_t target_row = (moved_tile - 1) / DIMENSITY;
        uint8_t target_col = (moved_tile - 1) % DIMENSITY;
    
        // —тара€ и нова€ позиции перемещенной плитки
        uint8_t old_row = new_empty_pos / DIMENSITY;
        uint8_t old_col = new_empty_pos % DIMENSITY;
        uint8_t new_row = old_empty_pos / DIMENSITY;
        uint8_t new_col = old_empty_pos % DIMENSITY;
    
        // ¬ычисл€ем изменение рассто€ни€
        short old_distance = abs(target_row - old_row) + abs(target_col - old_col);
        short new_distance = abs(target_row - new_row) + abs(target_col - new_col);
    
        return new_distance - old_distance;
    }

    static short calculateLinearDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) {
        uint8_t moved_tile = gsa.state[old_empty_pos]; // ѕлитка переместилась на место пустой

        // ¬ычисл€ем целевую позицию перемещенной плитки
        uint8_t target_row = (moved_tile - 1) / DIMENSITY;
        uint8_t target_col = (moved_tile - 1) % DIMENSITY;

        // —тара€ и нова€ позиции перемещенной плитки
        uint8_t old_row = new_empty_pos / DIMENSITY;
        uint8_t old_col = new_empty_pos % DIMENSITY;
        uint8_t new_row = old_empty_pos / DIMENSITY;
        uint8_t new_col = old_empty_pos % DIMENSITY;

        uint8_t tiles[DIMENSITY]{};

        short old_conflict = 0;
        short new_conflict = 0;
        //uint8_t count = 0;
        //uint8_t tile = 0;
        //uint8_t ind_moved_tile = 0;
        uint8_t count = 0, tile, ind_moved_tile;
        if (old_col == new_col) {
            if (old_row == target_row) {
                for (uint8_t col = 0; col < DIMENSITY; ++col) {

                    if (col == old_col) {
                        ind_moved_tile = count;
                        tiles[count++] = moved_tile;
                    }
                    else {
                        uint8_t pos = old_row * DIMENSITY + col;
                        tile = gsa.state[pos];
                        uint8_t targ_row = (tile - 1) / DIMENSITY;
                        if (targ_row == old_row)
                            tiles[count++] = tile;
                    }

                }

                for (uint8_t i = 0; i < count; ++i) {
                    for (uint8_t j = i + 1; j < count; ++j) {
                        if (tiles[i] > tiles[j]) {
                            ++old_conflict;
                            if (i != ind_moved_tile && j != ind_moved_tile)
                                ++new_conflict;
                        }
                    }
                }
            }
            else if (new_row == target_row) {
                for (uint8_t col = 0; col < DIMENSITY; ++col) {

                    if (col == old_col) {
                        ind_moved_tile = count;
                        tiles[count++] = moved_tile;
                    }
                    else {
                        uint8_t pos = new_row * DIMENSITY + col;
                        tile = gsa.state[pos];
                        uint8_t targ_row = (tile - 1) / DIMENSITY;
                        if (targ_row == new_row)
                            tiles[count++] = tile;
                    }
                }

                for (uint8_t i = 0; i < count; ++i) {
                    for (uint8_t j = i + 1; j < count; ++j) {
                        if (tiles[i] > tiles[j]) {
                            ++new_conflict;
                            if (i != ind_moved_tile && j != ind_moved_tile)
                                ++old_conflict;
                        }
                    }
                }
            }
        }
        else if (old_row == new_row) {
            if (old_col == target_col) {
                for (uint8_t row = 0; row < DIMENSITY; ++row) {

                    if (row == old_row) {
                        ind_moved_tile = count;
                        tiles[count++] = moved_tile;
                    }
                    else {
                        uint8_t pos = row * DIMENSITY + old_col;
                        tile = gsa.state[pos];
                        uint8_t targ_col = (tile - 1) % DIMENSITY;
                        if (targ_col == old_col)
                            tiles[count++] = tile;
                    }

                }

                for (uint8_t i = 0; i < count; ++i) {
                    for (uint8_t j = i + 1; j < count; ++j) {
                        if (tiles[i] > tiles[j]) {
                            ++old_conflict;
                            if (i != ind_moved_tile && j != ind_moved_tile)
                                ++new_conflict;
                        }
                    }
                }
            }
            else if (new_col == target_col) {
                for (uint8_t row = 0; row < DIMENSITY; ++row) {

                    if (row == new_row) {
                        ind_moved_tile = count;
                        tiles[count++] = moved_tile;
                    }
                    else {
                        uint8_t pos = row * DIMENSITY + new_col;
                        tile = gsa.state[pos];
                        uint8_t targ_col = (tile - 1) % DIMENSITY;
                        if (targ_col == new_col)
                            tiles[count++] = tile;
                    }
                }

                for (uint8_t i = 0; i < count; ++i) {
                    for (uint8_t j = i + 1; j < count; ++j) {
                        if (tiles[i] > tiles[j]) {
                            ++new_conflict;
                            if (i != ind_moved_tile && j != ind_moved_tile)
                                ++old_conflict;
                        }
                    }
                }
            }
        }

        return (new_conflict - old_conflict) << 1;
    }

    static short calculateCornerDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) { return 0; }


    static short calculateLinearConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) {
        return calculateManhattanDelta(gsa, new_empty_pos, old_empty_pos) + calculateLinearDelta(gsa, new_empty_pos, old_empty_pos);
    }

    static short calculateCornerConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) {
        return calculateManhattanDelta(gsa, new_empty_pos, old_empty_pos) + calculateCornerDelta(gsa, new_empty_pos, old_empty_pos);
    }

    static short calculateCornerLinearConflict(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos) {
        return calculateLinearDelta(gsa, new_empty_pos, old_empty_pos) + calculateCornerDelta(gsa, new_empty_pos, old_empty_pos);
    }
    
    short calculateHeuristicDelta(const GameStateArray& gsa, uint8_t new_empty_pos, uint8_t old_empty_pos, Heuristic heuristic_type) {
        switch (heuristic_type) {
        case Heuristic::MANHATTAN:
            return calculateManhattanDelta(gsa, new_empty_pos, old_empty_pos);
        case Heuristic::LINEAR_CONFLICT:
            return calculateLinearConflict(gsa, new_empty_pos, old_empty_pos);
        case Heuristic::CORNER_CONFLICTS:
            return calculateCornerConflict(gsa, new_empty_pos, old_empty_pos);
        case Heuristic::CORNER_LINEAR_CONFLICTS:
            return calculateCornerLinearConflict(gsa, new_empty_pos, old_empty_pos);
        default:
            return calculateManhattanDelta(gsa, new_empty_pos, old_empty_pos);
        }
    }

}