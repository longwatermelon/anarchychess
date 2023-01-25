#include "board.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <SDL2/SDL_image.h>

Board::Board(SDL_Renderer *rend, const std::string &board_fp)
    : m_selected(-1, -1)
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
    std::vector<Coord> moves = get_valid_moves(m_selected);

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
                SDL_RenderCopyF(rend, m_textures[m_board[y][x]], nullptr, &rtile);

            if (std::find(moves.begin(), moves.end(), Coord(x, y)) != moves.end())
            {
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(rend, 0, 255, 0, 100);
                SDL_RenderFillRectF(rend, &rtile);
                SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
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

void Board::select(Coord c)
{
    m_selected = c;
}

void Board::set_tile_size(float size)
{
    m_tile_size = size;
}

std::vector<Coord> Board::get_valid_moves(Coord from) const
{
    std::vector<Coord> moves;

    auto pawn_capture = [this, from](Coord c){
        return c.valid() && at(c) != '.' && color_at(c) != color_at(from);
    };

    Coord c = from;
    switch (at(from))
    {
    case 'P':
    {
        --c.y;
        if (c.valid() && at(c) == '.')
        {
            moves.emplace_back(c);

            if (from.y == 6)
            {
                --c.y;
                if (c.valid() && at(c) == '.')
                    moves.emplace_back(c);
                ++c.y;
            }
        }

        --c.x;
        if (pawn_capture(c)) moves.emplace_back(c);

        c.x += 2;
        if (pawn_capture(c)) moves.emplace_back(c);
    } break;
    case 'p':
    {
        ++c.y;
        if (c.valid() && at(c) == '.')
        {
            moves.emplace_back(c);

            if (from.y == 1)
            {
                ++c.y;
                if (c.valid() && at(c) == '.')
                    moves.emplace_back(c);
                --c.y;
            }
        }

        --c.x;
        if (pawn_capture(c)) moves.emplace_back(c);

        c.x += 2;
        if (pawn_capture(c)) moves.emplace_back(c);
    } break;
    }

    return moves;
}

void Board::scan_valid(Coord from, int dx, int dy, std::vector<Coord> &moves) const
{
    Coord c = from;
    while (c.valid())
    {
        if (color_at(c) != Color::NONE)
        {
            if (color_at(c) != color_at(from))
                moves.emplace_back(c);

            break;
        }

        moves.emplace_back(c);
        c.x += dx;
        c.y += dy;
    }
}

char Board::at(Coord c) const
{
    if (!c.valid()) return '.';
    return m_board[c.y][c.x];
}

Color Board::color_at(Coord c) const
{
    if (at(c) == '.')
        return Color::NONE;

    return at(c) < 'a' ? Color::WHITE : Color::BLACK;
}

