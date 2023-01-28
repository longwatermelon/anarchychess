#include "prog.h"
#include "ai.h"
#include <sstream>

Prog::Prog(SDL_Window *w, SDL_Renderer *r)
    : m_win(w), m_rend(r), m_board(r, "res/board")
{
    m_font = TTF_OpenFont("res/font.ttf", 50);
    m_small_font = TTF_OpenFont("res/font.ttf", 16);
}

Prog::~Prog()
{
    TTF_CloseFont(m_font);
    TTF_CloseFont(m_small_font);
}

void Prog::mainloop()
{
    SDL_Event evt;
    int wx = 800, wy = 800;

    SDL_Texture *wcheckmate = render_text(m_font, "Black wins by checkmate (Click to restart)");
    SDL_Texture *bcheckmate = render_text(m_font, "White wins by checkmate (Click to restart)");
    SDL_Rect rtext;
    SDL_QueryTexture(wcheckmate, nullptr, nullptr, &rtext.w, &rtext.h);

#ifndef __EMSCRIPTEN__
    while (m_running)
#endif
    {
        /* SDL_GetWindowSize(m_win, &wx, &wy); */
        float tile_size = std::min(wx, wy) / 8.f;
        SDL_FPoint top_left = wx > wy ?
                SDL_FPoint{ (wx - tile_size * 8.f) / 2.f, 0.f } :
                SDL_FPoint{ 0.f, (wy - tile_size * 8.f) / 2.f };
        int board_side = tile_size * 8.f;
        top_left.y += 40;

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                m_running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
            {
                int mx, my;
                SDL_GetMouseState(&mx, &my);

                if (m_game_over)
                {
                    m_restart = true;
                    m_running = false;
                }
                else
                {
                    if (m_playing)
                    {
                        int x = (mx - top_left.x) / tile_size;
                        int y = (my - top_left.y) / tile_size;
                        Move prev_move = m_board.last_move();
                        float eval = ai::eval(m_board);
                        m_board.select(Coord(x, y));

                        if (m_board.last_move() != prev_move)
                        {
                            std::stringstream ss;
                            ss.precision(2);
                            ss << std::fixed << ai::eval(m_board) - eval;
                            m_eval_diff = ss.str();
                        }
                    }
                    else
                    {
                        if (my < wy / 2)
                            m_board.clear_anarchy_moves();

                        m_playing = true;
                    }
                }
            } break;
            }
        }

        float prev_w = rtext.w;
        rtext.w = .7f * board_side;
        rtext.h = rtext.w / prev_w * rtext.h;
        rtext.x = (int)top_left.x + board_side / 2 - rtext.w / 2;
        rtext.y = (int)top_left.y + board_side / 2 - rtext.h / 2;

        if (m_board.turn() == Color::BLACK && !m_board.in_animation() && !m_board.detect_checkmate(Color::BLACK))
        {
            float eval = ai::eval(m_board);

            if (rand() % 100 < 5)
                m_board.move(ai::random_move(m_board));
            else
                m_board.move(ai::minimax_root(m_board, 3));

            std::stringstream ss;
            ss.precision(2);
            ss << std::fixed << ai::eval(m_board) - eval;
            m_eval_diff = ss.str();
        }

        SDL_RenderClear(m_rend);

        m_board.set_tile_size(tile_size);
        m_board.render(m_rend, top_left);

        SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 100);
        if (m_board.detect_checkmate(Color::WHITE))
        {
            SDL_RenderFillRect(m_rend, nullptr);
            SDL_RenderCopy(m_rend, wcheckmate, nullptr, &rtext);
            m_game_over = true;
        }

        if (m_board.detect_checkmate(Color::BLACK))
        {
            SDL_RenderFillRect(m_rend, nullptr);
            SDL_RenderCopy(m_rend, bcheckmate, nullptr, &rtext);
            m_game_over = true;
        }
        SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_NONE);

        if (!m_playing)
        {
            int mx, my;
            SDL_GetMouseState(&mx, &my);

            SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_BLEND);
            SDL_Rect rtop = { (int)top_left.x, (int)top_left.y, board_side, board_side / 2 };
            SDL_Rect rbot = { (int)top_left.x, (int)top_left.y + board_side / 2, board_side, board_side / 2 };

            SDL_SetRenderDrawColor(m_rend, 0, 100, 0, 100);
            SDL_RenderFillRect(m_rend, my < wy / 2 ? &rtop : &rbot);
            SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 100);
            SDL_RenderFillRect(m_rend, my >= wy / 2 ? &rtop : &rbot);

            SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_NONE);
            SDL_Texture *normal = render_text(m_font, "Play computer (Normal)");
            SDL_Texture *anarchy = render_text(m_font, "Play computer (Anarchy)");

            SDL_Rect rn, ra;
            SDL_QueryTexture(normal, nullptr, nullptr, &rn.w, &rn.h);
            SDL_QueryTexture(anarchy, nullptr, nullptr, &ra.w, &ra.h);

            float rw = rn.w;
            rn.w = .5f * board_side;
            rn.h = rn.w / rw * rn.h;
            rn.x = (int)top_left.x + board_side / 2 - rn.w / 2;
            rn.y = (int)top_left.y + board_side / 4 - rn.h / 2;

            rw = ra.w;
            ra.w = .5f * board_side;
            ra.h = ra.w / rw * ra.h;
            ra.x = (int)top_left.x + board_side / 2 - ra.w / 2;
            ra.y = (int)top_left.y + board_side / 4 - ra.h / 2 + board_side / 2;

            SDL_RenderCopy(m_rend, normal, nullptr, &rn);
            SDL_RenderCopy(m_rend, anarchy, nullptr, &ra);

            SDL_DestroyTexture(normal);
            SDL_DestroyTexture(anarchy);
        }

        // Render game info
        {
            std::stringstream ss;
            ss.precision(2);
            ss << "Eval: " << std::fixed << ai::eval(m_board) << " (" << (m_eval_diff[0] == '-' ? m_eval_diff : std::string("+") + m_eval_diff) << ")";
            SDL_Texture *eval = render_text(m_small_font, ss.str());
            SDL_Rect r = { .x = 10, .y = 10 };
            SDL_QueryTexture(eval, nullptr, nullptr, &r.w, &r.h);
            SDL_RenderCopy(m_rend, eval, nullptr, &r);
            SDL_DestroyTexture(eval);

            std::stringstream ss_move;
            if (!m_board.last_move().name.empty())
                ss_move << m_board.last_move().name << " to " << m_board.last_move().to.to_str();
            SDL_Texture *last_move = render_text(m_small_font, ss_move.str());
            SDL_QueryTexture(last_move, nullptr, nullptr, &r.w, &r.h);
            r.x = wx - r.w - 10;
            SDL_RenderCopy(m_rend, last_move, nullptr, &r);
            SDL_DestroyTexture(last_move);

            SDL_Texture *to_move = render_text(m_small_font, std::string(m_board.turn() == Color::WHITE ? "White" : "Black") + " to move");
            SDL_QueryTexture(to_move, nullptr, nullptr, &r.w, &r.h);
            r.x = wx / 2 - r.w / 2;
            SDL_RenderCopy(m_rend, to_move, nullptr, &r);
            SDL_DestroyTexture(to_move);
        }

        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
        SDL_RenderPresent(m_rend);
    }

    SDL_DestroyTexture(wcheckmate);
    SDL_DestroyTexture(bcheckmate);
}

SDL_Texture *Prog::render_text(TTF_Font *font, std::string s)
{
    if (s.empty()) s = " ";
    SDL_Surface *surf = TTF_RenderText_Blended(font, s.c_str(), { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(m_rend, surf);
    SDL_FreeSurface(surf);

    return tex;
}

