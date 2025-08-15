#ifndef MINIMAXMULTITHREADBOT_H
#define MINIMAXMULTITHREADBOT_H

#include "AbstractBot.h"

class MinimaxMultithreadBot final : public AbstractBot {
    void order_moves(std::vector<std::tuple<int, int, int>> &moves, Color self_turn, bool ascending) const;
    [[nodiscard]] std::pair<std::tuple<int, int, int>, int> minimax_thread(
        Color self_turn, Color current_turn, int depth,
        int alpha, int beta, std::vector<std::tuple<int, int, int>> moves = {});
    [[nodiscard]] std::pair<std::tuple<int, int, int>, int> minimax_YBW(Color self_turn);
public:
    MinimaxMultithreadBot() = delete;
    using AbstractBot::AbstractBot;
    std::tuple<int, int, int> bestmove(std::string command) override;
};

#endif //MINIMAXMULTITHREADBOT_H
