#include "ai.h"

std::vector<Move> ai::get_all_moves(Board &board, Color c)
{
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (board.color_at(Coord(x, y)) == c)
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

    if (board.detect_checkmate(Color::WHITE))
        return -1e5;
    if (board.detect_checkmate(Color::BLACK))
        return 1e5;

    return total;
}

Move ai::random_move(Board &board)
{
    std::vector<Move> moves = get_all_moves(board, Color::BLACK);
    return moves[rand() % moves.size()];
}

Move ai::best_move(Board &board)
{
    std::vector<Move> moves = get_all_moves(board, Color::BLACK);

    float best_eval = eval(board);
    Move best_move = moves[0];

    auto prev_board = board.get_board();
    for (const auto &move : moves)
    {
        board.test_move(move);
        if (eval(board) < best_eval)
        {
            best_move = move;
            best_eval = eval(board);
        }
        board.restore_saved_board(prev_board);
    }

    return best_move;
}

Move ai::minimax_root(Board &board, int depth)
{
    std::vector<Move> moves = get_all_moves(board, Color::BLACK);

    float best_eval = -1e5f;
    Move best = moves[rand() % moves.size()];

    auto prev_board = board.get_board();
    for (const auto &move : moves)
    {
        board.test_move(move);
        float move_eval = minimax(board, depth - 1, false);
        board.restore_saved_board(prev_board);

        if (move_eval > best_eval)
        {
            best_eval = move_eval;
            best = move;
        }
    }

    return best;
}

float ai::minimax(Board &board, int depth, bool maximizing_player)
{
    if (depth == 0)
        return -eval(board);

    std::vector<Move> moves = get_all_moves(board, maximizing_player ? Color::BLACK : Color::WHITE);
    float best_eval = 1e5f * (maximizing_player ? -1.f : 1.f);

    auto prev_board = board.get_board();
    for (const auto &move : moves)
    {
        board.test_move(move);
        if (!maximizing_player)
            best_eval = std::min(best_eval, minimax(board, depth - 1, !maximizing_player));
        else
            best_eval = std::max(best_eval, minimax(board, depth - 1, !maximizing_player));
        board.restore_saved_board(prev_board);
    }

    return best_eval;
}

