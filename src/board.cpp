#include "board.h"

Board::Board()
{
}

Board::~Board()
{
}

void Board::render(SDL_Renderer *rend, SDL_FPoint top_left)
{
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (std::abs(x - y) % 2 == 0)
                SDL_SetRenderDrawColor(rend, 240, 250, 250, 255);
            else
                SDL_SetRenderDrawColor(rend, 83, 170, 173, 255);

            SDL_FRect rtile = { top_left.x + x * m_tile_size,
                                top_left.y + y * m_tile_size,
                                m_tile_size, m_tile_size };

            SDL_RenderFillRectF(rend, &rtile);
        }
    }
}

bool Board::move(Coord from, Coord to)
{
    if (from == to)
        return false;

    return true;
}

void Board::set_tile_size(float size)
{
    m_tile_size = size;
}

