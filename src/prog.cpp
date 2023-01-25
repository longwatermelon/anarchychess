#include "prog.h"

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

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                m_running = false;
                break;
            }
        }

        SDL_RenderClear(m_rend);

        float tile_size = std::min(wx, wy) / 8.f;
        m_board.set_tile_size(tile_size);
        m_board.render(m_rend,
            wx > wy ?
                SDL_FPoint{ (wx - tile_size * 8.f) / 2.f, 0.f } :
                SDL_FPoint{ 0.f, (wy - tile_size * 8.f) / 2.f });

        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
        SDL_RenderPresent(m_rend);
    }
}

