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

    bool running() const { return m_running; }
    bool should_restart() const { return m_restart; }

private:
    SDL_Texture *render_text(TTF_Font *font, std::string s);

private:
    bool m_running{ true };
    SDL_Window *m_win{ nullptr };
    SDL_Renderer *m_rend{ nullptr };
    TTF_Font *m_font{ nullptr }, *m_small_font{ nullptr };

    Board m_board;

    bool m_playing{ false };
    bool m_game_over{ false };
    bool m_restart{ false };

    std::string m_eval_diff;
};

