#ifndef MINIMAXBOT_H
#define MINIMAXBOT_H

#include "AbstractBot.h"
#include <random>

class MinimaxBot final : public AbstractBot {
    [[nodiscard]] std::pair<std::tuple<int, int, int>, int> minimax_search(
        Color self_turn, Color current_turn, int depth);
public:
    MinimaxBot() = delete;
    using AbstractBot::AbstractBot;
    std::tuple<int, int, int> bestmove(std::string command) override;
};


#endif //MINIMAXBOT_H
