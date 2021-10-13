#include "test-common.hpp"
#include "Vpattern_sequencer_tb.h"

#include <iostream>

void setClock(Vpattern_sequencer_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpattern_sequencer_tb;

#define HEADER(LEN, REPEAT, ADDR) (ADDR | (LEN << 8) | (REPEAT << 13))
#define NOTE(NOTE, LEN, INSTRUMENT) (NOTE | (LEN << 6) | (INSTRUMENT << 11))
#define PATTERN(NUMBER, LEN) (NUMBER | (LEN << 8))

struct PatternSequencerFixture : TestFixture<UUT>
{
    Input8 i_note_stb;
    Output8 o_note_valid;
    Output8 o_note_pitch;
    Output8 o_note_len;
    Output8 o_note_instrument;
    PatternSequencerFixture() :
        i_note_stb(makeInput(&UUT::i_note_stb)),
        o_note_valid(makeOutput(&UUT::o_note_valid)),
        o_note_pitch(makeOutput(&UUT::o_note_pitch)),
        o_note_len(makeOutput(&UUT::o_note_len)),
        o_note_instrument(makeOutput(&UUT::o_note_instrument))
    {
    }

    void setupNoteStrobe(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_note_stb.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }

    Vector8 allNotes(uint64_t endTime)
    {
        Vector8 allNotes;
        Vector8 noteTimeline = o_note_pitch.timeline(endTime);
        for (auto change : o_note_valid.changes()) {
            uint64_t time = std::get<0>(change);
            uint8_t value = std::get<1>(change);
            if (value == 1) {
                uint8_t note_at_time = noteTimeline[time];
                allNotes.push_back(note_at_time);
            }
        }
        return allNotes;
    }
};

using Fixture = PatternSequencerFixture;

TEST_CASE_METHOD(Fixture, "Single pattern stop", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(1, 0, 0x00),
        /* 0x01 */  PATTERN(0x02, 2),

        // Pattern 0
        /* 0x02 */  NOTE(05, 2, 3),
        /* 0x03 */  NOTE(15, 1, 9),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(50);

    bench.tick(50);

    CHECK(allNotes(50) == Vector8({05, 15}));
}

TEST_CASE_METHOD(Fixture, "Multi pattern stop", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(2, 0, 0x00),

        /* 0x01 */  PATTERN(0x03, 3),
        /* 0x02 */  PATTERN(0x06, 2),

        // Pattern 0
        /* 0x03 */  NOTE(0x05, 2, 0x3),
        /* 0x04 */  NOTE(0x15, 1, 0x9),
        /* 0x05 */  NOTE(0x10, 0, 0xB),

        // Patten 1
        /* 0x06 */  NOTE(0x08, 1, 0x1),
        /* 0x07 */  NOTE(0x04, 2, 0x2),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(75);

    bench.tick(75);

    CHECK(allNotes(75) == Vector8({
        0x05, 0x15, 0x10,
        0x08, 0x04,
    }));
}

TEST_CASE_METHOD(Fixture, "Single pattern repeat", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(1, 1, 0x00),
        /* 0x01 */  PATTERN(0x02, 2),

        // Pattern 0
        /* 0x02 */  NOTE(05, 2, 3),
        /* 0x03 */  NOTE(15, 1, 9),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(75);

    bench.tick(75);

    CHECK(allNotes(50) == Vector8({05, 15, 05, 15, 05, 15, 05}));
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
