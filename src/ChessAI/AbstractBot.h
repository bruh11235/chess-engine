#ifndef ABSTRACTBOT_H
#define ABSTRACTBOT_H

#include "../EngineCore/ChessEngine.h"
#include <string>
#include <random>

class AbstractBot {
protected:
    ChessEngine &engine;
    std::mt19937 rng;
public:
    explicit AbstractBot (ChessEngine &engine, const int seed = 0) : engine(engine), rng(std::mt19937(seed)) {}
    virtual ~AbstractBot() = default;
    virtual std::tuple<int, int, int> bestmove(std::string command) = 0;
};

#endif //ABSTRACTBOT_H
