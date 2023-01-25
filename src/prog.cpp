#include "prog.h"

Prog::Prog(SDL_Window *w, SDL_Renderer *r)
    : m_win(w), m_rend(r)
{
}

Prog::~Prog()
{
}

void Prog::mainloop()
{
    SDL_Event evt;

    while (m_running)
    {
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

        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
        SDL_RenderPresent(m_rend);
    }
}

