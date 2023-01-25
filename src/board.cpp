#include "board.h"
#include <sstream>
#include <fstream>
#include <SDL2/SDL_image.h>

Board::Board(SDL_Renderer *rend, const std::string &board_fp)
{
    std::ifstream ifs(board_fp);
    std::string buf;

    int y = 0;
    while (std::getline(ifs, buf))
    {
        for (int i = 0; i < 8; ++i)
            m_board[y][i] = buf[i];
        ++y;
    }

    m_textures['r'] = IMG_LoadTexture(rend, "res/b-rook.png");
    m_textures['n'] = IMG_LoadTexture(rend, "res/b-knight.png");
    m_textures['b'] = IMG_LoadTexture(rend, "res/b-bishop.png");
    m_textures['q'] = IMG_LoadTexture(rend, "res/b-queen.png");
    m_textures['k'] = IMG_LoadTexture(rend, "res/b-king.png");
    m_textures['p'] = IMG_LoadTexture(rend, "res/b-pawn.png");

    m_textures['R'] = IMG_LoadTexture(rend, "res/w-rook.png");
    m_textures['N'] = IMG_LoadTexture(rend, "res/w-knight.png");
    m_textures['B'] = IMG_LoadTexture(rend, "res/w-bishop.png");
    m_textures['Q'] = IMG_LoadTexture(rend, "res/w-queen.png");
    m_textures['K'] = IMG_LoadTexture(rend, "res/w-king.png");
    m_textures['P'] = IMG_LoadTexture(rend, "res/w-pawn.png");
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

            if (m_board[y][x] != '.')
            {
                SDL_RenderCopyF(rend, m_textures[m_board[y][x]], nullptr, &rtile);
            }
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

