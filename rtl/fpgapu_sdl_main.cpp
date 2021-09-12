#include <math.h>
#include <SDL.h>
#include <verilated.h>
#include "Vfpgapu_sdl_top.h"

using TopModule = Vfpgapu_sdl_top;

static TopModule * top = 0;
static const int SAMPLE_FREQ = 48000;

void callback(void* userdata, Uint8* stream, int len) {
    uint8_t * snd = reinterpret_cast<uint8_t *>(stream);
    len /= sizeof(*snd);
    for( int i = 0; i < len; i++) //Fill array with frequencies, mathy-math stuff
    {
        top->i_clk = 1;
        top->eval();
        top->i_clk = 0;
        top->eval();

        snd[i] = top->o_audio_sample;
    }
}

int main(int argc, char* argv[])
{
    Verilated::commandArgs(argc, argv);
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL init failed.\n");
        return 1;
    }

    SDL_Window * window = SDL_CreateWindow("FPGAPU", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, render_flags);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioSpec spec, aspec; // the specs of our piece of "music"
    SDL_zero(spec);
    spec.freq = SAMPLE_FREQ; //declare specs
    spec.format = AUDIO_U8;
    spec.channels = 1;
    spec.samples = 4096;
    spec.callback = callback;
    spec.userdata = NULL;

      //Open audio, if error, print
    SDL_AudioDeviceID id;
    if ((id = SDL_OpenAudioDevice(nullptr, 0, &spec, &aspec, SDL_AUDIO_ALLOW_ANY_CHANGE)) <= 0 )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open audio: %s\n", SDL_GetError());
        return 1;
    }

    if (spec.freq != aspec.freq) {
        SDL_Log("We didn't get sample frequency");
    }

    if (spec.format != aspec.format) {
        SDL_Log("We didn't get audio format");
    }

    /* Start playing, "unpause" */
    SDL_PauseAudioDevice(id, 0);

    top = new TopModule();
    fprintf(stderr, "PWD: %s\n", getcwd(NULL, 0));

    top->i_clk = 0;
    top->eval();

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                SDL_PauseAudioDevice(id, 1);
                running = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
