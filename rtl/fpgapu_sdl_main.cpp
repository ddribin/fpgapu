#include <math.h>
#include <SDL.h>
#include <verilated.h>
#include "Vfpgapu_sdl_top.h"

using TopModule = Vfpgapu_sdl_top;

static TopModule * top = 0;
static const uint64_t SAMPLE_FREQ = 441000;  // 44.1 kHz
static const uint64_t CLOCK_FREQ = 1000000;  // 1 MHz
static const uint8_t  COUNTER_WIDTH = 32;
static const uint64_t COUNTER_MASK = (1ULL << COUNTER_WIDTH) - 1;

static uint64_t counter;
static uint64_t callback_delta = 0;

void run_one_tick(void)
{
    top->i_clk = 1;
    top->eval();
    top->i_clk = 0;
    top->eval();
}

void run_until_wrap(void)
{
    while (1) {
        run_one_tick();

        counter += callback_delta;
        bool wrap = (counter >> COUNTER_WIDTH) != 0;
        counter &= COUNTER_MASK;
        if (wrap) {
            break;
        }
    }
}

void callback(void* userdata, Uint8* stream, int len) {
    uint8_t * snd = reinterpret_cast<uint8_t *>(stream);
    len /= sizeof(*snd);
    for( int i = 0; i < len; i++) {
        run_until_wrap();
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

    fprintf(stderr, "PWD: %s\n", getcwd(NULL, 0));

    double delta = SAMPLE_FREQ * pow(2, COUNTER_WIDTH)/CLOCK_FREQ;
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Callback delta: %f", delta);

    bool mute_1 = false;
    bool mute_2 = false;
    bool mute_3 = false;
    bool mute_4 = false;
    bool reset = false;

    // These are accessed in the audio callback, which is a separate thread, so need to lock it.
    // Probably not needed since the device is still paused, but this ensures proper memory barriers.
    SDL_LockAudioDevice(id);
    top = new TopModule();
    top->i_clk = 0;
    top->i_mixer = 0b1111;
    top->eval();

    callback_delta = roundl(delta);
    SDL_UnlockAudioDevice(id);

    /* Start playing, "unpause" */
    SDL_PauseAudioDevice(id, 0);

    bool running = true;
    while (running) {
        uint8_t mixer =
            ((mute_4? 0 : 1 << 3) |
             (mute_3? 0 : 1 << 2) |
             (mute_2? 0 : 1 << 1) |
             (mute_1? 0 : 1 << 0));
        
        SDL_LockAudioDevice(id);
        top->i_mixer = mixer;
        top->i_rst = reset;
        SDL_UnlockAudioDevice(id);

        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                SDL_PauseAudioDevice(id, 1);
                running = false;
            } else if (e.type == SDL_KEYDOWN) {
                int key = e.key.keysym.sym;
                switch (key) {
                    case SDLK_1:
                        mute_1 = !mute_1;
                        break;

                    case SDLK_2:
                        mute_2 = !mute_2;
                        break;

                    case SDLK_3:
                        mute_3 = !mute_3;
                        break;

                    case SDLK_4:
                        mute_4 = !mute_4;
                        break;

                    case SDLK_r:
                        reset = true;
                        break;

                    default:
                        break;
                }
            } else if (e.type == SDL_KEYUP) {
                int key = e.key.keysym.sym;
                switch (key) {
                    case SDLK_r:
                        reset = false;
                        break;

                    default:
                        break;
                }

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
