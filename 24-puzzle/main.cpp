#include <iostream>
#include <fstream>
#include <functional>
#include "Heuristic.h"
#include "GameState.h"
#include "Info.h"
#include "Solution.h"
#include "Solver.h"

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
    //"12345678A0BE9FCD", // 19 
    //"F2345678A0BE91DC", // 33
    //"75AB2C416D389F0E", // 45
    //"FE169B4C0A73D852", // 52
    //"D79F2E8A45106C3B", // 55
    "BAC0F478E19623D5", // 61
};

vector<const char*> vec5 = {
    //"123456789ABCDEFGHIJ0LMNOK", // 1
    "123456789ABCDEFGHIJKLMN0O", // 1
    //"F63M09LE51BO27IDNJACH48KG", // 1
    //"1234K6709ABC85JGHDEFLMINO", // 18
    //"1234KH7N9AB065ELGC8JMIDOF", // 39
    "1234F0678ANHDEOBCKJIGLM95", // 39
    //"12853E7F9K0BN6DCAHLJGMI4O", // 39
    //"D7IMLOB2H18N4035CG9KFJ6EA", //
    //"K12HACBO6G9E4L63JM07FDNI5", //
    //"DGC8A25HIJLM0ENB67F19KO43", //
    //"JOHIF9AL8B2E063G7M5K4CD1N", //
};

Algorithm algorithms[] = {
    //Algorithm("A* Manhattan", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::MANHATTAN); }),
    Algorithm("A* linear", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::LINEAR_CONFLICT); }),
    Algorithm("A* corner", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::CORNER_CONFLICTS); }),
    Algorithm("A* corner&linear", [](const char* state, int& explored) { return solveAStar(state, explored, Heuristic::CORNER_LINEAR_CONFLICTS); }),
    Algorithm("IDA* Manhattan", [](const char* state, int& explored) { return solveIDAStar(state, explored, Heuristic::MANHATTAN); }),
    Algorithm("IDA* linear", [](const char* state, int& explored) { return solveIDAStar(state, explored, Heuristic::LINEAR_CONFLICT); }),
    Algorithm("IDA* corner", [](const char* state, int& explored) { return solveIDAStar(state, explored, Heuristic::CORNER_CONFLICTS); }),
    Algorithm("IDA* corner&linear", [](const char* state, int& explored) { return solveIDAStar(state, explored, Heuristic::CORNER_LINEAR_CONFLICTS); }),
};

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