#include "prog.h"
#include "ai.h"

Prog::Prog(SDL_Window *w, SDL_Renderer *r)
    : m_win(w), m_rend(r), m_board(r, "res/board")
{
    m_font = TTF_OpenFont("res/font.ttf", 50);
}

Prog::~Prog()
{
    TTF_CloseFont(m_font);
}

void Prog::mainloop()
{
    SDL_Event evt;
    int wx, wy;

    SDL_Texture *wcheckmate = render_text("Black wins by checkmate (Click to restart)");
    SDL_Texture *bcheckmate = render_text("White wins by checkmate (Click to restart)");
    SDL_Rect rtext;
    SDL_QueryTexture(wcheckmate, nullptr, nullptr, &rtext.w, &rtext.h);

#ifndef __EMSCRIPTEN__
    while (m_running)
#endif
    {
        SDL_GetWindowSize(m_win, &wx, &wy);
        float tile_size = std::min(wx, wy) / 8.f;
        SDL_FPoint top_left = wx > wy ?
                SDL_FPoint{ (wx - tile_size * 8.f) / 2.f, 0.f } :
                SDL_FPoint{ 0.f, (wy - tile_size * 8.f) / 2.f };

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
                        m_board.select(Coord(x, y));
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
        rtext.w = .7f * wx;
        rtext.h = rtext.w / prev_w * rtext.h;
        rtext.x = wx / 2 - rtext.w / 2;
        rtext.y = wy / 2 - rtext.h / 2;

        if (m_board.turn() == Color::BLACK && !m_board.in_animation() && !m_board.detect_checkmate(Color::BLACK))
        {
            if (rand() % 100 < 5)
                m_board.move(ai::random_move(m_board));
            else
                m_board.move(ai::minimax_root(m_board, 4));
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
            SDL_Rect rtop = { 0, 0, wx, wy / 2 };
            SDL_Rect rbot = { 0, wy / 2, wx, wy / 2 };

            SDL_SetRenderDrawColor(m_rend, 0, 100, 0, 100);
            SDL_RenderFillRect(m_rend, my < wy / 2 ? &rtop : &rbot);
            SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 100);
            SDL_RenderFillRect(m_rend, my >= wy / 2 ? &rtop : &rbot);

            SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_NONE);
            SDL_Texture *normal = render_text("Play computer (Normal)");
            SDL_Texture *anarchy = render_text("Play computer (Anarchy)");

            SDL_Rect rn, ra;
            SDL_QueryTexture(normal, nullptr, nullptr, &rn.w, &rn.h);
            SDL_QueryTexture(anarchy, nullptr, nullptr, &ra.w, &ra.h);

            float rw = rn.w;
            rn.w = .5f * wx;
            rn.h = rn.w / rw * rn.h;
            rn.x = wx / 2 - rn.w / 2;
            rn.y = wy / 4 - rn.h / 2;

            rw = ra.w;
            ra.w = .5f * wx;
            ra.h = ra.w / rw * ra.h;
            ra.x = wx / 2 - ra.w / 2;
            ra.y = wy / 4 - ra.h / 2 + wy / 2;

            SDL_RenderCopy(m_rend, normal, nullptr, &rn);
            SDL_RenderCopy(m_rend, anarchy, nullptr, &ra);

            SDL_DestroyTexture(normal);
            SDL_DestroyTexture(anarchy);
        }

        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
        SDL_RenderPresent(m_rend);
    }

    SDL_DestroyTexture(wcheckmate);
    SDL_DestroyTexture(bcheckmate);
}

SDL_Texture *Prog::render_text(std::string s)
{
    if (s.empty()) s = " ";
    SDL_Surface *surf = TTF_RenderText_Blended(m_font, s.c_str(), { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(m_rend, surf);
    SDL_FreeSurface(surf);

    return tex;
}

