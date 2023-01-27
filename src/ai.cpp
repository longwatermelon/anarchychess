#include "ai.h"

Move ai::best_move(Board &board, Color turn)
{
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (board.color_at(Coord(x, y)) == turn)
            {
                std::vector<Move> piece_moves = board.get_valid_moves(Coord(x, y), false);
                if (!piece_moves.empty())
                    moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
            }
        }
    }

    return moves[rand() % moves.size()];
}

