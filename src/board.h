#pragma once
#include <array>
#include <string>
#include <vector>
#include <unordered_map>
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

    Move(Coord from, Coord to)
        : from(from), to(to) {}
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
    std::vector<Coord> get_valid_moves(Coord from, bool raw);
    void scan_valid(Coord from, int dx, int dy, std::vector<Coord> &moves, bool raw);
    void add_valid_move(std::vector<Coord> &moves, Coord from, Coord to, bool raw);

    char at(Coord c) const;
    Color color_at(Coord c) const;

private:
    std::array<std::array<char, 8>, 8> m_board;
    float m_tile_size{ 0.f };

    std::unordered_map<char, SDL_Texture*> m_textures;

    Coord m_selected;

    std::vector<Animation> m_animations;
};

