#include "Info.h"
#include "Solver.h"

using namespace std;

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

void writeFullSolveInOutput(const char start_state[SIZE_OF_FIELD], const Solution& solution, ofstream& output, const string& algorithm_name) {
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

void result(const char input[SIZE_OF_FIELD], const Solution& solution, const string& algorithm_name,
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
