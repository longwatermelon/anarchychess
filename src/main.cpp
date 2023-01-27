#include "prog.h"
#include <ctime>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

Prog *g_prog;

void run()
{
    g_prog->mainloop();
}

int main(int argc, char **argv)
{
    srand(time(0));

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Window *win = SDL_CreateWindow("Anarchy chess",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            600, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    {
        g_prog = new Prog(win, rend);
#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(run, -1, 1);
#else
        g_prog->mainloop();
#endif
        delete g_prog;
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}

