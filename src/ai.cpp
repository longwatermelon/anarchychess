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

    static std::unordered_map<char, std::vector<std::vector<float>>> pos_points = {
        {'P', {
            {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
            {5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0},
            {1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0},
            {0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5},
            {0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0},
            {0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5},
            {0.5,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  0.5},
            {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0}
        }},
        {'N', {
            {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
            {-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
            {-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
            {-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
            {-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
            {-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
            {-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
            {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
        }},
        {'B', {
            { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
            { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0},
            { -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0},
            { -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0},
            { -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0},
            { -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0},
            { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
        }},
        {'R', {
            {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
            {  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            {  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0}
        }},
        {'K', {
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
            { -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
            {  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 },
            {  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 }
        }},
        {'Q', {
            { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},
            { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
            { -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
            {  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
            { -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}
        }},

        {'p', {
            {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
            {0.5,  1.0, 1.0,  -2.0, -2.0,  1.0,  1.0,  0.5},
            {0.5, -0.5, -1.0,  0.0,  0.0, -1.0, -0.5,  0.5},
            {0.5,  0.5,  1.0,  2.5,  2.5,  1.0,  0.5,  0.5},
            {1.0,  1.0,  2.0,  3.0,  3.0,  2.0,  1.0,  1.0},
            {0.0,  0.0,  0.0,  2.0,  2.0,  0.0,  0.0,  0.0},
            {5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0,  5.0},
            {0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0}
        }},
        {'n', {
            {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0},
            {-4.0, -2.0,  0.0,  0.0,  0.0,  0.0, -2.0, -4.0},
            {-3.0,  0.0,  1.0,  1.5,  1.5,  1.0,  0.0, -3.0},
            {-3.0,  0.5,  1.5,  2.0,  2.0,  1.5,  0.5, -3.0},
            {-3.0,  0.0,  1.5,  2.0,  2.0,  1.5,  0.0, -3.0},
            {-3.0,  0.5,  1.0,  1.5,  1.5,  1.0,  0.5, -3.0},
            {-4.0, -2.0,  0.0,  0.5,  0.5,  0.0, -2.0, -4.0},
            {-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0}
        }},
        {'b', {
            { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0},
            { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  1.0,  1.0,  0.5,  0.0, -1.0},
            { -1.0,  0.5,  0.5,  1.0,  1.0,  0.5,  0.5, -1.0},
            { -1.0,  0.0,  1.0,  1.0,  1.0,  1.0,  0.0, -1.0},
            { -1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0, -1.0},
            { -1.0,  0.5,  0.0,  0.0,  0.0,  0.0,  0.5, -1.0},
            { -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0}
        }},
        {'r', {
            {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
            {  0.5,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            { -0.5,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -0.5},
            {  0.0,   0.0, 0.0,  0.5,  0.5,  0.0,  0.0,  0.0}
        }},
        {'k', {
            {  2.0,  3.0,  1.0,  0.0,  0.0,  1.0,  3.0,  2.0 },
            {  2.0,  2.0,  0.0,  0.0,  0.0,  0.0,  2.0,  2.0 },
            { -1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0},
            { -2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0},
            { -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0}
        }},
        {'q', {
            { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0},
            { -1.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
            { -0.5,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
            {  0.0,  0.0,  0.5,  0.5,  0.5,  0.5,  0.0, -0.5},
            { -1.0,  0.5,  0.5,  0.5,  0.5,  0.5,  0.0, -1.0},
            { -1.0,  0.0,  0.5,  0.0,  0.0,  0.0,  0.0, -1.0},
            { -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0}
        }}
    };

    float total = 0.f;

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (board.color_at(Coord(x, y)) == Color::WHITE)
                total += points[board.at(Coord(x, y)) - 'A' + 'a'] + pos_points[board.at(Coord(x, y))][y][x];
            else if (board.color_at(Coord(x, y)) == Color::BLACK)
                total -= points[board.at(Coord(x, y))] + pos_points[board.at(Coord(x, y))][y][x];
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
        float move_eval = minimax(board, depth - 1, -1e5f, 1e5f, false);
        board.restore_saved_board(prev_board);

        if (move_eval > best_eval)
        {
            best_eval = move_eval;
            best = move;
        }
    }

    return best;
}

float ai::minimax(Board &board, int depth, float alpha, float beta, bool maximizing_player)
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
        {
            best_eval = std::min(best_eval, minimax(board, depth - 1, alpha, beta, !maximizing_player));
            beta = std::min(beta, best_eval);
            if (beta <= alpha)
                return best_eval;
        }
        else
        {
            best_eval = std::max(best_eval, minimax(board, depth - 1, alpha, beta, !maximizing_player));
            alpha = std::max(alpha, best_eval);
            if (beta <= alpha)
                return best_eval;
        }
        board.restore_saved_board(prev_board);
    }

    return best_eval;
}

