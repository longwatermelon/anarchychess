#pragma once
#include "board.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class Prog
{
public:
    Prog(SDL_Window *w, SDL_Renderer *r);
    ~Prog();

    void mainloop();

private:
    SDL_Texture *render_text(std::string s);

private:
    bool m_running{ true };
    SDL_Window *m_win{ nullptr };
    SDL_Renderer *m_rend{ nullptr };
    TTF_Font *m_font{ nullptr };

    Board m_board;

    bool m_playing{ false };
};

