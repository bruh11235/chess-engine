#ifndef ABSTRACTBOT_H
#define ABSTRACTBOT_H

#include "../EngineCore/ChessEngine.h"
#include <string>

class AbstractBot {
protected:
    ChessEngine &engine;
public:
    explicit AbstractBot (ChessEngine &engine) : engine(engine) {}
    virtual ~AbstractBot() = default;
    virtual std::tuple<int, int, int> bestmove(std::string command) = 0;
};

#endif //ABSTRACTBOT_H
