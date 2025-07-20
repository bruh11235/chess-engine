#ifndef GREEDYBOT_H
#define GREEDYBOT_H

#include "AbstractBot.h"
#include <random>

class GreedyBot final : public AbstractBot {
public:
    GreedyBot() = delete;
    using AbstractBot::AbstractBot;
    std::tuple<int, int, int> bestmove(std::string command) override;
};

#endif //GREEDYBOT_H
