#ifndef RANDOMBOT_H
#define RANDOMBOT_H

#include "AbstractBot.h"
#include <random>

class RandomBot final : public AbstractBot {
public:
    RandomBot() = delete;
    using AbstractBot::AbstractBot;
    std::tuple<int, int, int> bestmove(std::string command) override;
};

#endif //RANDOMBOT_H
