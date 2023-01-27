#pragma once
#include "board.h"

namespace ai
{
    std::vector<Move> get_all_moves(Board &board, Color c);

    float eval(Board &board);

    Move random_move(Board &board);
    Move best_move(Board &board);

    Move minimax_root(Board &board, int depth);
    float minimax(Board &board, int depth, Color turn);
}

