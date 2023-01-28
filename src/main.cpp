#include "prog.h"
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

Prog *g_prog;
SDL_Window *g_win;
SDL_Renderer *g_rend;

void run()
{
#ifndef __EMSCRIPTEN__
    while (true)
#endif
    {
        g_prog->mainloop();

        if (!g_prog->running())
        {
            bool restart = g_prog->should_restart();
            delete g_prog;

            if (!restart)
            {
#ifdef __EMSCRIPTEN__
                emscripten_cancel_main_loop();
#else
                return;
#endif
            }
            g_prog = new Prog(g_win, g_rend);
        }
    }
}

int main(int argc, char **argv)
{
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    g_win = SDL_CreateWindow("Anarchy chess",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800, 840, SDL_WINDOW_SHOWN);
    g_rend = SDL_CreateRenderer(g_win, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    g_prog = new Prog(g_win, g_rend);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(run, -1, 1);
#else
    run();
#endif

    SDL_DestroyRenderer(g_rend);
    SDL_DestroyWindow(g_win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

