#pragma once
#include <array>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <SDL2/SDL.h>

struct Coord
{
    int x, y;

    Coord(int x, int y)
        : x(x), y(y) {}

    std::string to_str() const
    {
        return std::string(1, 'a' + x) + std::to_string((7 - y) + 1);
    }

    bool valid() const
    {
        return x >= 0 && x <= 7 &&
               y >= 0 && y <= 7;
    }

    bool operator==(const Coord &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coord &other) const
    {
        return !(*this == other);
    }
};

class Board;

struct Move
{
    std::string name;
    Coord from;
    Coord to;

    bool special{ false };
    std::function<void(Move)> special_move_fn;

    Move(const Board &board, Coord from, Coord to);

    bool operator==(const Move &other) const
    {
        return from == other.from && to == other.to;
    }

    bool operator!=(const Move &other) const
    {
        return !(*this == other);
    }
};

struct SpecialMove
{
    std::string name;
    std::function<bool(Coord, Coord&)> cond;
    std::function<void(Move)> move_fn;
    bool anarchy{ true };
};

enum class Color
{
    NONE,
    WHITE,
    BLACK
};

struct Animation
{
    SDL_Texture *tex;
    Coord from, to;
    Uint32 time_ms, begin;
};

class Board
{
public:
    Board(SDL_Renderer *rend, const std::string &board_fp);
    ~Board();

    void render(SDL_Renderer *rend, SDL_FPoint top_left);

    void move(Move move);

    std::vector<Move> get_valid_moves(Coord from, bool raw);

    bool detect_check(Color c);
    bool detect_checkmate(Color c);

    void select(Coord c);
    void set_tile_size(float size);

    std::array<std::array<char, 8>, 8> get_board();
    void test_move(Move move);
    void restore_saved_board(const std::array<std::array<char, 8>, 8> &board);

    char at(Coord c) const;
    Color color_at(Coord c) const;

    Color turn() const { return m_turn; }

    bool in_animation() const { return !m_animations.empty(); }
    Move last_move() const { return m_last_move; }

    void clear_anarchy_moves();
    bool anarchy() const { return m_anarchy; }

private:
    bool move_internal(Move move);

    void scan_valid(Coord from, int dx, int dy, std::vector<Move> &moves, bool raw, int n, char up_to = ' ');
    void add_valid_move(std::vector<Move> &moves, Move move, bool raw);

private:
    std::array<std::array<char, 8>, 8> m_board;
    float m_tile_size{ 0.f };

    std::unordered_map<char, SDL_Texture*> m_textures;

    Coord m_selected;

    std::vector<Animation> m_animations;
    std::vector<SpecialMove> m_special_moves;

    Move m_last_move;
    Color m_turn{ Color::WHITE };

    bool m_anarchy{ true };

    Uint32 m_explode_begin = 0;
    SDL_Texture *m_texplode{ nullptr };
};

