#ifndef UCIHANDLER_H
#define UCIHANDLER_H

#include "ChessEngine.h"
#include <string>

class UCIHandler {
    ChessEngine &engine;
    void read_starting_position(const std::string &command) const;
public:
    explicit UCIHandler(ChessEngine &engine) : engine(engine) {}
    void start() const;
};

#endif //UCIHANDLER_H
