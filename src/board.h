#pragma once
#include <array>
#include <string>
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
};

class Board
{
public:
    Board();
    ~Board();

    void render(SDL_Renderer *rend, SDL_FPoint top_left);

    bool move(Coord from, Coord to);

    void set_tile_size(float size);

private:
    std::array<std::array<char, 8>, 8> m_board;
    float m_tile_size{ 0.f };
};

