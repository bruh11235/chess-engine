#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "AbstractBot.h"
#include <random>

class RandomBot final : public AbstractBot {
    std::mt19937 rng;
public:
    RandomBot() = delete;
    explicit RandomBot (ChessEngine &engine, int seed = 0);
    std::tuple<int, int, int> bestmove(std::string command) override;
};

#endif //RANDOMBOT_H
