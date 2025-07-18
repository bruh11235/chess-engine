#ifndef UCIHANDLER_H
#define UCIHANDLER_H

#include "ChessEngine.h"
#include "../ChessAI/AbstractBot.h"
#include <string>

class UCIHandler {
    ChessEngine &engine;
    AbstractBot &bot;
    void read_starting_position(const std::string &command) const;
    void get_move(const std::string &command) const;
public:
    explicit UCIHandler(ChessEngine &engine, AbstractBot &bot) : engine(engine), bot(bot) {}
    void start() const;
};

#endif //UCIHANDLER_H
