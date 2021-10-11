#include "test-common.hpp"
#include "Vpattern_sequencer_tb.h"

void setClock(Vpattern_sequencer_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpattern_sequencer_tb;

#define NOTE(NOTE, LEN, INSTRUMENT) (NOTE | (LEN << 6) | (INSTRUMENT << 11))
#define PATTERN(NUMBER, LEN) (NUMBER | (LEN << 8))

struct PatternSequencerFixture : TestFixture<UUT>
{
    Input8 i_note_stb;
    PatternSequencerFixture() :
        i_note_stb(makeInput(&UUT::i_note_stb))
    {
    static uint16_t ROM[] = {
        /* 0x00 */  PATTERN(0x02, 3),
        /* 0x01 */  PATTERN(0x05, 2),

        // Pattern 0
        /* 0x02 */  NOTE(0x05, 2, 0x3),
        /* 0x03 */  NOTE(0x15, 1, 0x9),
        /* 0x04 */  NOTE(0x10, 0, 0xB),

        // Patten 1
        /* 0x05 */  NOTE(0x08, 1, 0x1),
        /* 0x06 */  NOTE(0x04, 2, 0x2),
    };
        memcpy(core.zz_memory, ROM, sizeof(ROM));
    }

    void setupNoteStrobe(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_note_stb.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }
};

using Fixture = PatternSequencerFixture;

TEST_CASE_METHOD(Fixture, "Pattern sequencer initial outputs", "[pattern-seq]")
{
    setupNoteStrobe(30);

    bench.tick(50);
}

static void state_machine(void)
{
    static uint16_t ROM[] = {
        /* 0x00 */  PATTERN(0x02, 3),
        /* 0x01 */  PATTERN(0x05, 2),

        // Pattern 0
        /* 0x02 */  NOTE(0x05, 2, 0x3),
        /* 0x03 */  NOTE(0x15, 1, 0x9),
        /* 0x04 */  NOTE(0x10, 0, 0xB),

        // Patten 1
        /* 0x05 */  NOTE(0x08, 1, 0x1),
        /* 0x06 */  NOTE(0x04, 2, 0x2),
    };

#if 0
    uint16_t pattern_addr = 0x00;
    for (int pattern = 0; pattern < 2; pattern++) {
        uint16_t pattern_info = ROM[pattern_addr];
        uint16_t pattern_start = pattern_info & 0xFF;
        uint16_t pattern_len = (pattern_info >> 8) & 0xFF;

        uint16_t note_addr = pattern_start;
        for (int note = 0; note < pattern_len; note++) {
            uint16_t note_info = ROM[note_addr];
        }
    }
#endif

    typedef enum {
        STATE_IDLE
    } State;

    static bool started = false;
    static uint16_t pattern_addr = 0x00;
    static uint16_t note_addr = 0x00;
    if (!started) {
        uint16_t pattern_info = ROM[pattern_addr];
        uint16_t pattern_start = pattern_info & 0xFF;
        uint16_t pattern_len = (pattern_info >> 8) & 0xFF;
        note_addr = pattern_start;
    }
}
