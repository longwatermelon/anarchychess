#include "prog.h"
#include "ai.h"

Prog::Prog(SDL_Window *w, SDL_Renderer *r)
    : m_win(w), m_rend(r), m_board(r, "res/board")
{
}

Prog::~Prog()
{
}

void Prog::mainloop()
{
    SDL_Event evt;
    int wx, wy;

    while (m_running)
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

                int x = (mx - top_left.x) / tile_size;
                int y = (my - top_left.y) / tile_size;
                m_board.select(Coord(x, y));
            } break;
            }
        }

        if (m_board.detect_checkmate(Color::WHITE))
            printf("White is in checkmate\n");

        if (m_board.detect_checkmate(Color::BLACK))
        {
            printf("Black is in checkmate\n");
        }
        else
        {
            if (m_board.turn() == Color::BLACK && !m_board.in_animation() && !m_board.detect_checkmate(Color::BLACK))
            {
                m_board.move(ai::minimax_root(m_board, 3));
            }
        }

        SDL_RenderClear(m_rend);

        m_board.set_tile_size(tile_size);
        m_board.render(m_rend, top_left);

        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
        SDL_RenderPresent(m_rend);
    }
}

