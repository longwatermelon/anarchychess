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
        return std::to_string('a' + x) + std::to_string(y + 1);
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

struct Move
{
    Coord from;
    Coord to;

    bool special{ false };
    std::function<void(Move)> special_move_fn;

    Move(Coord from, Coord to)
        : from(from), to(to) {}

    bool operator==(const Move &other) const
    {
        return from == other.from && to == other.to;
    }
};

struct SpecialMove
{
    std::string name;
    std::function<bool(Coord, Coord&)> cond;
    std::function<void(Move)> move_fn;
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

    bool move(Move move);

    bool detect_check(Color c);

    void select(Coord c);
    void set_tile_size(float size);

private:
    std::vector<Move> get_valid_moves(Coord from, bool raw);
    void scan_valid(Coord from, int dx, int dy, std::vector<Move> &moves, bool raw, int n);
    void add_valid_move(std::vector<Move> &moves, Move move, bool raw);

    char at(Coord c) const;
    Color color_at(Coord c) const;

private:
    std::array<std::array<char, 8>, 8> m_board;
    float m_tile_size{ 0.f };

    std::unordered_map<char, SDL_Texture*> m_textures;

    Coord m_selected;

    std::vector<Animation> m_animations;
    std::vector<SpecialMove> m_special_moves;
};

