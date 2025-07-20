#ifndef MINIMAXBOT_H
#define MINIMAXBOT_H

#include "AbstractBot.h"
#include <random>

class MinimaxBot final : public AbstractBot {
    void order_moves(std::vector<std::tuple<int, int, int>> &moves, Color self_turn, bool ascending) const;
    [[nodiscard]] std::pair<std::tuple<int, int, int>, int> minimax_search(
        Color self_turn, Color current_turn, int depth, int alpha, int beta);

public:
    MinimaxBot() = delete;
    using AbstractBot::AbstractBot;
    std::tuple<int, int, int> bestmove(std::string command) override;
};


#endif //MINIMAXBOT_H
