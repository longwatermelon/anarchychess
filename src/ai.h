#pragma once
#include "board.h"

namespace ai
{
    std::vector<Move> get_all_moves(Board &board);

    float eval(Board &board);

    Move random_move(Board &board);
    Move best_move(Board &board);
}

