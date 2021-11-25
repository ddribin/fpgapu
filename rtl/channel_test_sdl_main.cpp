#include <math.h>
#include <SDL.h>
#include <verilated.h>
#include "Vchannel_test_sdl_top.h"

using TopModuleBase = Vchannel_test_sdl_top;
// using TopModule = Vchannel_test_sdl_top;


#define TOPVAR(A)     channel_test_sdl_top__DOT__ ## A

#define	CHTESTVAR(A)   TOPVAR(channel_test__DOT__ ## A)

#define	CHCTRLVAR(A)   CHTESTVAR(channel_controller__DOT__ ## A)

// #define chctrl_state    CHCTRLVAR(state)
#define chctrl_pattern_enable     CHCTRLVAR(pattern_enable)

#define	SEQVAR(A)   CHTESTVAR(pattern_sequencer__DOT__ ## A)
// #define seq_state   SEQVAR(state)
#define seq_note_pitch SEQVAR(note_pitch)
#define seq_note_len SEQVAR(note_len)
#define seq_note_instrument SEQVAR(note_instrument)
// #define seq_rom_addr SEQVAR(rom_addr)
#define seq_pattern_addr SEQVAR(pattern_addr)
#define seq_pattern_addr_nxt SEQVAR(pattern_addr_nxt)
#define	PITCHVAR(A)   CHTESTVAR(pitch_lookup__DOT__ ## A)
#define	DURVAR(A)   CHTESTVAR(duration_counter__DOT__ ## A)

// #define pattern_rom_mem CHTESTVAR(pattern_rom__DOT__memory)

class TopModule : public TopModuleBase {
    public:
    using TopModuleBase::TopModuleBase;

    uint8_t chctrl_state() const { return CHCTRLVAR(state); }

    CData seq_state() const { return SEQVAR(state); }
    CData seq_rom_addr() const { return SEQVAR(rom_addr); }

    const SData* pattern_rom_mem() const { return CHTESTVAR(pattern_rom__DOT__memory); }
    const uint32_t pitch_phase_delta() const { return PITCHVAR(phase_delta); }
    const uint8_t duration_duration() const { return DURVAR(duration); }
    const uint32_t r_phase_delta() const { return CHTESTVAR(r_phase_delta); }
    const uint32_t r_phase_delta_nxt() const { return CHTESTVAR(r_phase_delta_nxt); }
};

// Called by $time in Verilog
// converts to double, to match
// what SystemC does
double sc_time_stamp (void)
{
    return 0;
}

static VerilatedContext * context = 0;
static TopModule * top = 0;
static const uint64_t SAMPLE_FREQ = 441000;  // 44.1 kHz
static const uint64_t CLOCK_FREQ = 1000000;  // 1 MHz
static const uint8_t  COUNTER_WIDTH = 32;
static const uint64_t COUNTER_MASK = (1ULL << COUNTER_WIDTH) - 1;

static uint64_t counter;
static uint64_t callback_delta = 0;

static uint64_t beat_count = 0;

void run_one_tick(void)
{
    top->i_clk = 1;
    top->eval();
    context->timeInc(5);
    top->i_clk = 0;
    top->eval();
    context->timeInc(5);
}

void nop(void) {

}

void run_until_wrap(void)
{
    while (1) {
        run_one_tick();

        if (top->o_beat) {
            beat_count++;
        }

#if 0
        if (top->o_pattern_enable) {
            printf("%lu: Note: %d %d %d\n", context->time(), top->o_pitch, top->o_duration, top->o_instrument);
            printf("Channel State: %d\n", top->chctrl_state);
        }
#endif

#if 0
        struct timespec time;
        clock_gettime(CLOCK_REALTIME, &time);

        // if (true) {
        if ((top->o_sample_valid) || (top->o_tick)) {
            printf("%lld.%.6ld: %4lld: tick: %d, beat: %d, valid: %d, state: %d note_pitch: 0x%02x, note_len: %d, note_instrument: %d, delta: 0x%08X, r_delta: 0x%08x, r_delta_nxt: 0x%08X, phase: 0x%08X, duration: %d, sample: %d\n",
                (long long)time.tv_sec, time.tv_nsec/1000, beat_count,
                top->o_tick, top->o_beat, top->o_sample_valid,
                top->chctrl_state(), top->seq_note_pitch, top->seq_note_len, top->seq_note_instrument, top->pitch_phase_delta(), top->r_phase_delta(), top->r_phase_delta_nxt(), top->o_phase, top->duration_duration(), top->o_audio_sample);
            nop();
        }
#endif

#if 0
        int seq_state = top->seq_state();
        // if ((seq_state != 0) && (seq_state != 1) && (seq_state != 6)) {
        //     printf("Sequencer state: %d rom_addr: 0x%02x 0x%04x 0x%02x 0x%02x\n",
        //         seq_state, top->seq_rom_addr(), top->pattern_rom_mem()[top->seq_rom_addr()], top->seq_pattern_addr_nxt, top->seq_pattern_addr);
        // }

        if (seq_state == 2) {
            printf("Sequencer state: %d header addr: 0x%x\n",
                seq_state, top->seq_rom_addr);
        } else
        if (seq_state == 4) {
            printf("Sequencer state: %d order addr: 0x%x\n",
                seq_state, top->seq_rom_addr);
        } else
        if (seq_state == 7) {
            printf("Sequencer state: %d pattern addr: 0x%x\n",
                seq_state, top->seq_rom_addr);
        } else
        if (seq_state == 9) {
            printf("Sequencer state: %d note_pitch: 0x%x, note_len: %d, note_instrument: %d\n",
                top->seq_state, top->seq_note_pitch, top->seq_note_len, top->seq_note_instrument);
        }
#endif

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
        uint32_t phase = top->o_phase;
        // uint8_t sample_calc = (phase & 0x80000000)? 128+16 : 128-16;
        uint8_t sample_calc = (phase & 0x80000000)? 16 : 0;
        // printf("Phase: 0x%08X, sample_calc: %d, sample: %d\n", phase, sample_calc, top->o_audio_sample);
        snd[i] = top->o_audio_sample;
        // snd[i] = top->o_audio_sample;
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

    SDL_Window * window = SDL_CreateWindow("Channel Test", SDL_WINDOWPOS_CENTERED,
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

    bool reset = false;

    // These are accessed in the audio callback, which is a separate thread, so need to lock it.
    // Probably not needed since the device is still paused, but this ensures proper memory barriers.
    SDL_LockAudioDevice(id);
    context = new VerilatedContext();
    top = new TopModule(context);
    top->i_clk = 0;
    top->eval();

    callback_delta = roundl(delta);
    SDL_UnlockAudioDevice(id);

    /* Start playing, "unpause" */
    SDL_PauseAudioDevice(id, 0);

    bool running = true;
    while (running) {
        SDL_LockAudioDevice(id);
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
