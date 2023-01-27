#include "ai.h"

std::vector<Move> ai::get_all_moves(Board &board)
{
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (board.color_at(Coord(x, y)) == Color::BLACK)
            {
                std::vector<Move> piece_moves = board.get_valid_moves(Coord(x, y), false);
                if (!piece_moves.empty())
                    moves.insert(moves.end(), piece_moves.begin(), piece_moves.end());
            }
        }
    }

    return moves;
}

float ai::eval(Board &board)
{
    static std::unordered_map<char, float> points = {
        { 'p', 1.f },
        { 'n', 3.f },
        { 'b', 3.f },
        { 'r', 5.f },
        { 'o', 7.f },
        { 'q', 10.f },
        { 'k', 0.f }
    };

    float total = 0.f;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (board.color_at(Coord(x, y)) == Color::WHITE)
                total += points[board.at(Coord(x, y)) - 'A' + 'a'];
            else if (board.color_at(Coord(x, y)) == Color::BLACK)
                total -= points[board.at(Coord(x, y))];
        }
    }

    return total;
}

Move ai::random_move(Board &board)
{
    std::vector<Move> moves = get_all_moves(board);
    return moves[rand() % moves.size()];
}

Move ai::best_move(Board &board)
{
    std::vector<Move> moves = get_all_moves(board);

    float best_eval = eval(board);
    Move best_move = moves[rand() % moves.size()];

    board.save_board_state();
    for (const auto &move : moves)
    {
        board.test_move(move);
        if (eval(board) < best_eval)
        {
            best_move = move;
            best_eval = eval(board);
        }
        board.restore_saved_board();
    }

    return best_move;
}

