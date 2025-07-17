#include "UCIHandler.h"
#include <bits/stdc++.h>
using namespace std;


void UCIHandler::read_starting_position(const string &command) const {
    istringstream iss(command);
    string token;
    iss >> token;
    if (token != "position") return;

    // Read starting position
    iss >> token;
    if (token == "fen") {
        string fen_string;
        while (iss >> token) {
            if (token == "moves") break;
            fen_string += token + " ";
        }
        engine = ChessEngine(fen_string);
    } else if (token == "startpos") {
        iss >> token;
    } else {
        throw runtime_error("Unknown token after \'position\'");
    }

    // Read additional moves
    while (iss >> token) {
        if (token.length() != 4 && token.length() != 5) {
            throw runtime_error("Unknown moves");
        }
        const int file1 = static_cast<int>(token[0]) - 'a';
        const int rank1 = 8 - static_cast<int>(token[1]) + '0';
        const int file2 = static_cast<int>(token[2]) - 'a';
        const int rank2 = 8 - static_cast<int>(token[3]) + '0';
        const int index1 = rank1 * 8 + file1, index2 = rank2 * 8 + file2;

        if (token.length() == 4) {
            engine.move(index1, index2);
        } else {
            if (token[4] == 'n') engine.move(index1, index2, 1);
            else if (token[4] == 'b') engine.move(index1, index2, 2);
            else if (token[4] == 'r') engine.move(index1, index2, 3);
            else if (token[4] == 'q') engine.move(index1, index2, 4);
            else throw runtime_error("Unknown moves");
        }
    }
}


void UCIHandler::start() const {
    while (true) {
        string command;
        getline(cin, command);
        if (command == "uci") {
            cout << "uciok" << endl;
        } else if (command == "isready") {
            cout << "readyok" << endl;
        } else if (command.substr(0, 8) == "position") {
            read_starting_position(command);
        } else if (command.substr(0, 2) == "go") {
            cout << "bestmove e2e4" << endl;
        } else if (command == "quit") {
            break;
        }
    }
}
