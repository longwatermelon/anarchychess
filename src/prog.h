#pragma once
#include "board.h"
#include <SDL2/SDL.h>

class Prog
{
public:
    Prog(SDL_Window *w, SDL_Renderer *r);
    ~Prog();

    void mainloop();

private:
    bool m_running{ true };
    SDL_Window *m_win{ nullptr };
    SDL_Renderer *m_rend{ nullptr };

    Board m_board;
};

