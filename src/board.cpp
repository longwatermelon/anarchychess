#include "board.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <SDL2/SDL_image.h>

Move::Move(const Board &board, Coord from, Coord to)
    : from(from), to(to)
{
    switch (board.at(from))
    {
    case 'p': case 'P':
        name = "Pawn";
        break;
    case 'r': case 'R':
        name = "Rook";
        break;
    case 'n': case 'N':
        name = "Knight";
        break;
    case 'b': case 'B':
        name = "Bishop";
        break;
    case 'q': case 'Q':
        name = "Queen";
        break;
    case 'k': case 'K':
        name = "King";
        break;
    case 'o': case 'O':
        name = "Knook";
        break;
    }
}

Board::Board(SDL_Renderer *rend, const std::string &board_fp)
    : m_selected(-1, -1), m_last_move(*this, Coord(-1, -1), Coord(-1, -1))
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
    m_textures['o'] = IMG_LoadTexture(rend, "res/b-knook.png");

    m_textures['R'] = IMG_LoadTexture(rend, "res/w-rook.png");
    m_textures['N'] = IMG_LoadTexture(rend, "res/w-knight.png");
    m_textures['B'] = IMG_LoadTexture(rend, "res/w-bishop.png");
    m_textures['Q'] = IMG_LoadTexture(rend, "res/w-queen.png");
    m_textures['K'] = IMG_LoadTexture(rend, "res/w-king.png");
    m_textures['P'] = IMG_LoadTexture(rend, "res/w-pawn.png");
    m_textures['O'] = IMG_LoadTexture(rend, "res/w-knook.png");

    m_texplode = IMG_LoadTexture(rend, "res/explode.png");

    m_special_moves.emplace_back(SpecialMove{
        .name = "Short castle",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(6, 7);
            return (c == Coord(4, 7) && at(Coord(7, 7)) == 'R') &&
                   (at(Coord(5, 7)) == '.' && at(Coord(6, 7)) == '.');
        },
        .move_fn = [this](Move m){
            move_internal(Move(*this, Coord(7, 7), Coord(5, 7)));
            move_internal(Move(*this, Coord(4, 7), Coord(6, 7)));
        },
        .anarchy = false
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Short castle",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(6, 0);
            return (c == Coord(4, 0) && at(Coord(7, 0)) == 'r') &&
                   (at(Coord(5, 0)) == '.' && at(Coord(6, 0)) == '.');
        },
        .move_fn = [this](Move m){
            move_internal(Move(*this, Coord(7, 0), Coord(5, 0)));
            move_internal(Move(*this, Coord(4, 0), Coord(6, 0)));
        },
        .anarchy = false
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Long castle",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(2, 7);
            return (c == Coord(4, 7) && at(Coord(0, 7)) == 'R') &&
                   (at(Coord(1, 7)) == '.' && at(Coord(2, 7)) == '.' &&
                    at(Coord(3, 7)) == '.');
        },
        .move_fn = [this](Move m){
            move_internal(Move(*this, Coord(0, 7), Coord(3, 7)));
            move_internal(Move(*this, Coord(4, 7), Coord(2, 7)));
        },
        .anarchy = false
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Long castle",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(2, 0);
            return (c == Coord(4, 0) && at(Coord(0, 0)) == 'r') &&
                   (at(Coord(1, 0)) == '.' && at(Coord(2, 0)) == '.' &&
                    at(Coord(3, 0)) == '.');
        },
        .move_fn = [this](Move m){
            move_internal(Move(*this, Coord(0, 0), Coord(3, 0)));
            move_internal(Move(*this, Coord(4, 0), Coord(2, 0)));
        },
        .anarchy = false
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Knook spawn",
        .cond = [this](Coord c, Coord &disp){
            if (at(c) != 'N')
                return false;

            for (int y = std::max(0, c.y - 2); y <= std::min(7, c.y + 2); ++y)
            {
                for (int x = std::max(0, c.x - 2); x <= std::min(7, c.x + 2); ++x)
                {
                    if (std::abs((y - c.y) * (x - c.x)) == 2 && color_at(Coord(x, y)) == color_at(c))
                    {
                        if (at(Coord(x, y)) == 'R')
                        {
                            disp = Coord(x, y);
                            return true;
                        }
                    }
                }
            }

            return false;
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
            m_board[m.to.y][m.to.x] = 'O';
        }
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Knook spawn",
        .cond = [this](Coord c, Coord &disp){
            if (at(c) != 'n')
                return false;

            for (int y = std::max(0, c.y - 2); y <= std::min(7, c.y + 2); ++y)
            {
                for (int x = std::max(0, c.x - 2); x <= std::min(7, c.x + 2); ++x)
                {
                    if (std::abs((y - c.y) * (x - c.x)) == 2 && color_at(Coord(x, y)) == color_at(c))
                    {
                        if (at(Coord(x, y)) == 'r')
                        {
                            disp = Coord(x, y);
                            return true;
                        }
                    }
                }
            }

            return false;
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
            m_board[m.to.y][m.to.x] = 'o';
        }
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Siberian Swipe",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(0, 0);
            return (c == Coord(0, 7) && at(Coord(0, 7)) == 'R' && at(Coord(0, 0)) == 'r');
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
        }
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Siberian Swipe",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(7, 0);
            return (c == Coord(7, 7) && at(Coord(7, 7)) == 'R' && at(Coord(7, 0)) == 'r');
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
        }
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Siberian Swipe",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(0, 7);
            return (c == Coord(0, 0) && at(Coord(0, 0)) == 'r' && at(Coord(0, 7)) == 'R');
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
        }
    });

    m_special_moves.emplace_back(SpecialMove{
        .name = "Siberian Swipe",
        .cond = [this](Coord c, Coord &disp){
            disp = Coord(7, 7);
            return (c == Coord(7, 0) && at(Coord(7, 0)) == 'r' && at(Coord(7, 7)) == 'R');
        },
        .move_fn = [this](Move m){
            m.special = false;
            move_internal(m);
        }
    });
}

Board::~Board()
{
    for (auto &[key, value] : m_textures)
        SDL_DestroyTexture(value);

    SDL_DestroyTexture(m_texplode);
}

void Board::render(SDL_Renderer *rend, SDL_FPoint top_left)
{
    std::vector<Move> moves;
    get_valid_moves(m_selected, false, moves);

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (std::abs(x - y) % 2 == 0)
                SDL_SetRenderDrawColor(rend, 238, 239, 211, 255);
            else
                SDL_SetRenderDrawColor(rend, 119, 150, 87, 255);

            SDL_FRect rtile = { top_left.x + x * m_tile_size,
                                top_left.y + y * m_tile_size,
                                m_tile_size, m_tile_size };

            SDL_RenderFillRectF(rend, &rtile);

            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
            if (Coord(x, y) == m_last_move.from || Coord(x, y) == m_last_move.to)
            {
                SDL_SetRenderDrawColor(rend, 255, 255, 0, 100);
                SDL_RenderFillRectF(rend, &rtile);
            }

            if (std::find(moves.begin(), moves.end(), Move(*this, m_selected, Coord(x, y))) != moves.end())
            {
                SDL_SetRenderDrawColor(rend, 0, 255, 0, 100);
                SDL_RenderFillRectF(rend, &rtile);
            }
            SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
        }
    }

    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (m_board[y][x] != '.')
            {
                SDL_FRect rpiece = { top_left.x + x * m_tile_size,
                                     top_left.y + y * m_tile_size,
                                     m_tile_size, m_tile_size };

                for (size_t i = 0; i < m_animations.size(); ++i)
                {
                    if (SDL_GetTicks() - m_animations[i].begin > m_animations[i].time_ms)
                    {
                        if (m_anarchy && m_animations[i].to.to_str() == "c4")
                        {
                            m_explode_begin = SDL_GetTicks();
                            m_board[m_animations[i].to.y][m_animations[i].to.x] = '.';
                        }

                        m_animations.erase(m_animations.begin() + i--);
                        continue;
                    }

                    if (m_animations[i].to == Coord(x, y))
                    {
                        float percent = (float)(SDL_GetTicks() - m_animations[i].begin) / 150.f;

                        SDL_FPoint from = {
                            top_left.x + (m_animations[i].from.x * m_tile_size),
                            top_left.y + (m_animations[i].from.y * m_tile_size)
                        };
                        SDL_FPoint to = {
                            top_left.x + (m_animations[i].to.x * m_tile_size),
                            top_left.y + (m_animations[i].to.y * m_tile_size)
                        };

                        rpiece.x = from.x + percent * (to.x - from.x);
                        rpiece.y = from.y + percent * (to.y - from.y);
                    }
                }

                SDL_RenderCopyF(rend, m_textures[m_board[y][x]], nullptr, &rpiece);
            }
        }
    }

    if (SDL_GetTicks() - m_explode_begin <= 500 && m_explode_begin != 0)
    {
        SDL_FRect r = {
            top_left.x + 2 * m_tile_size,
            top_left.y + 4 * m_tile_size,
            m_tile_size,
            m_tile_size
        };

        SDL_RenderCopyF(rend, m_texplode, nullptr, &r);
    }
}

void Board::move(Move move)
{
    select(move.from);
    select(move.to);
}

bool Board::move_internal(Move move)
{
    if (move.special)
    {
        move.special_move_fn(move);
    }
    else
    {
        m_board[move.to.y][move.to.x] = at(move.from);
        m_board[move.from.y][move.from.x] = '.';

        if (move.to.y == 0 && at(move.to) == 'P')
            m_board[move.to.y][move.to.x] = m_anarchy ? 'O' : 'Q';

        if (move.to.y == 7 && at(move.to) == 'p')
            m_board[move.to.y][move.to.x] = m_anarchy ? 'o' : 'q';

        m_animations.emplace_back(Animation{
            .tex = m_textures[at(move.to)],
            .from = move.from,
            .to = move.to,
            .time_ms = 150,
            .begin = SDL_GetTicks()
        });

        m_last_move = move;
    }

    return true;
}

bool Board::detect_check(Color c)
{
    // Find king
    Coord king(-1, -1);
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (at(Coord(x, y)) == (c == Color::WHITE ? 'K' : 'k'))
            {
                king = Coord(x, y);
                goto breakloop;
            }
        }
    }

breakloop:
    // Detect check
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            Coord coord(x, y);
            if (at(coord) != '.' && color_at(coord) != c)
            {
                std::vector<Move> moves;
                get_valid_moves(coord, true, moves);

                for (const auto &move : moves)
                {
                    if (move.to == king)
                        return true;
                }
            }
        }
    }

    return false;
}

bool Board::detect_checkmate(Color c)
{
    for (int y = 0; y < 8; ++y)
    {
        for (int x = 0; x < 8; ++x)
        {
            if (color_at(Coord(x, y)) == c)
            {
                std::vector<Move> moves;
                get_valid_moves(Coord(x, y), false, moves);
                if (!moves.empty())
                    return false;
            }
        }
    }

    return true;
}

void Board::select(Coord c)
{
    if (!m_animations.empty())
        return;

    if (m_selected == Coord(-1, -1) && at(c) == '.')
    {
        m_selected = Coord(-1, -1);
        return;
    }

    if (m_selected == Coord(-1, -1) && color_at(c) != Color::NONE && color_at(c) != m_turn)
        return;

    std::vector<Move> moves;
    get_valid_moves(m_selected, false, moves);

    for (auto &m : moves)
    {
        if (c == m.to)
        {
            bool boost = false;
            if (m.name == "Pawn" && (m.to.y == 0 || m.to.y == 7) && m_anarchy)
            {
                boost = true;
                m.name = "Knook boost";
            }

            move_internal(m);
            m_selected = Coord(-1, -1);

            if (!boost)
                m_turn = m_turn == Color::WHITE ? Color::BLACK : Color::WHITE;

            return;
        }
    }

    if (m_selected == Coord(-1, -1) || color_at(c) == m_turn)
        m_selected = c;

    if (at(c) == '.')
        m_selected = Coord(-1, -1);
}

void Board::set_tile_size(float size)
{
    m_tile_size = size;
}

std::array<std::array<char, 8>, 8> Board::get_board()
{
    return m_board;
}

void Board::test_move(Move move)
{
    m_board[move.to.y][move.to.x] = at(move.from);
    m_board[move.from.y][move.from.x] = '.';
}

void Board::restore_saved_board(const std::array<std::array<char, 8>, 8> &board)
{
    m_board = board;
}

void Board::get_valid_moves(Coord from, bool raw, std::vector<Move> &moves)
{
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
            add_valid_move(moves, Move(*this, from, c), raw);

            if (from.y == 6)
            {
                --c.y;
                if (c.valid() && at(c) == '.')
                    add_valid_move(moves, Move(*this, from, c), raw);
                ++c.y;
            }
        }

        --c.x;
        if (pawn_capture(c)) add_valid_move(moves, Move(*this, from, c), raw);

        c.x += 2;
        if (pawn_capture(c)) add_valid_move(moves, Move(*this, from, c), raw);
    } break;
    case 'p':
    {
        ++c.y;
        if (c.valid() && at(c) == '.')
        {
            add_valid_move(moves, Move(*this, from, c), raw);

            if (from.y == 1)
            {
                ++c.y;
                if (c.valid() && at(c) == '.')
                    add_valid_move(moves, Move(*this, from, c), raw);
                --c.y;
            }
        }

        --c.x;
        if (pawn_capture(c)) add_valid_move(moves, Move(*this, from, c), raw);

        c.x += 2;
        if (pawn_capture(c)) add_valid_move(moves, Move(*this, from, c), raw);
    } break;
    case 'r': case 'R':
        scan_valid(from, 1, 0, moves, raw, 8);
        scan_valid(from, 0, 1, moves, raw, 8);
        scan_valid(from, -1, 0, moves, raw, 8);
        scan_valid(from, 0, -1, moves, raw, 8);
        break;
    case 'b': case 'B':
        scan_valid(from, 1, 1, moves, raw, 8);
        scan_valid(from, -1, -1, moves, raw, 8);
        scan_valid(from, 1, -1, moves, raw, 8);
        scan_valid(from, -1, 1, moves, raw, 8);
        break;
    case 'q': case 'Q':
        if (m_anarchy)
        {
            std::vector<Move> tmp;
            scan_valid(from, 1, 1, tmp, raw, 8, 'k');
            scan_valid(from, -1, -1, tmp, raw, 8, 'k');
            scan_valid(from, 1, -1, tmp, raw, 8, 'k');
            scan_valid(from, -1, 1, tmp, raw, 8, 'k');

            for (auto &m : tmp)
            {
                m.name = "Alaskan Avalanche";
                m.special = true;
                m.special_move_fn = [this](Move m){
                    m.special = false;
                    move_internal(m);

                    int dx = m.to.x - m.from.x < 0 ? -1 : 1;
                    int dy = m.to.y - m.from.y < 0 ? -1 : 1;
                    while (true)
                    {
                        m.from.x += dx;
                        m.from.y += dy;
                        if (m.from == m.to)
                            break;
                        m_board[m.from.y][m.from.x] = '.';
                    }
                };
            }

            moves.insert(moves.end(), tmp.begin(), tmp.end());
        }
        else
        {
            scan_valid(from, 1, 1, moves, raw, 8);
            scan_valid(from, -1, -1, moves, raw, 8);
            scan_valid(from, 1, -1, moves, raw, 8);
            scan_valid(from, -1, 1, moves, raw, 8);
        }

        scan_valid(from, 1, 0, moves, raw, 8);
        scan_valid(from, 0, 1, moves, raw, 8);
        scan_valid(from, -1, 0, moves, raw, 8);
        scan_valid(from, 0, -1, moves, raw, 8);
        break;
    case 'n': case 'N':
    {
        for (int y = std::max(0, from.y - 2); y <= std::min(7, from.y + 2); ++y)
        {
            for (int x = std::max(0, from.x - 2); x <= std::min(7, from.x + 2); ++x)
            {
                if (std::abs((y - from.y) * (x - from.x)) == 2 && color_at(Coord(x, y)) != color_at(from))
                    add_valid_move(moves, Move(*this, from, Coord(x, y)), raw);
            }
        }
    } break;
    case 'k': case 'K':
    {
        for (int y = std::max(0, from.y - 1); y <= std::min(7, from.y + 1); ++y)
        {
            for (int x = std::max(0, from.x - 1); x <= std::min(7, from.x + 1); ++x)
            {
                if (color_at(Coord(x, y)) != color_at(from))
                    add_valid_move(moves, Move(*this, from, Coord(x, y)), raw);
            }
        }
    } break;
    case 'o': case 'O':
    {
        scan_valid(from, 1, 0, moves, raw, 2);
        scan_valid(from, 0, 1, moves, raw, 2);
        scan_valid(from, -1, 0, moves, raw, 2);
        scan_valid(from, 0, -1, moves, raw, 2);

        for (int y = std::max(0, from.y - 2); y <= std::min(7, from.y + 2); ++y)
        {
            for (int x = std::max(0, from.x - 2); x <= std::min(7, from.x + 2); ++x)
            {
                if (std::abs((y - from.y) * (x - from.x)) == 2 && color_at(Coord(x, y)) != color_at(from))
                    add_valid_move(moves, Move(*this, from, Coord(x, y)), raw);
            }
        }
    } break;
    }

    for (const auto &special : m_special_moves)
    {
        Coord disp(-1, -1);
        if (special.cond(from, disp))
        {
            Move m(*this, from, disp);
            auto prev_board = m_board;
            test_move(m);
            if (!detect_check(color_at(disp)))
            {
                m.special = true;
                m.special_move_fn = special.move_fn;
                m.name = special.name;
                moves.emplace_back(m);
            }
            m_board = prev_board;
        }
    }
}

void Board::scan_valid(Coord from, int dx, int dy, std::vector<Move> &moves, bool raw, int n, char up_to)
{
    Coord c = from;
    c.x += dx;
    c.y += dy;

    if (std::tolower(at(c)) == up_to)
        return;

    while (c.valid())
    {
        Coord next(c.x + dx, c.y + dy);
        if ((up_to == ' ' && color_at(c) != Color::NONE) || std::tolower(at(next)) == up_to)
        {
            if (color_at(c) != color_at(from))
                add_valid_move(moves, Move(*this, from, c), raw);

            break;
        }

        add_valid_move(moves, Move(*this, from, c), raw);
        c = next;
        --n;

        if (n <= 0)
            break;
    }
}

void Board::add_valid_move(std::vector<Move> &moves, Move move, bool raw)
{
    if (raw)
    {
        moves.emplace_back(move);
        return;
    }

    char tmp = at(move.to);
    m_board[move.to.y][move.to.x] = at(move.from);
    m_board[move.from.y][move.from.x] = '.';

    if (!detect_check(color_at(move.to)))
        moves.emplace_back(move);

    m_board[move.from.y][move.from.x] = at(move.to);
    m_board[move.to.y][move.to.x] = tmp;
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

void Board::clear_anarchy_moves()
{
    for (size_t i = 0; i < m_special_moves.size(); ++i)
    {
        if (m_special_moves[i].anarchy)
            m_special_moves.erase(m_special_moves.begin() + i--);
    }

    m_anarchy = false;
}

