#include "test-common.hpp"
#include "Vpattern_sequencer_tb.h"

#include <iostream>

void setClock(Vpattern_sequencer_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpattern_sequencer_tb;

#define HEADER(LAST_ADDR, REPEAT, REPEAT_ADDR) (LAST_ADDR | (REPEAT_ADDR << 6) | (REPEAT << 12))
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

TEST_CASE_METHOD(Fixture, "pattern: Single pattern stop", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x01, 0, 0x00),
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

TEST_CASE_METHOD(Fixture, "pattern: Multi pattern stop", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x02, 0, 0x00),

        /* 0x01 */  PATTERN(0x03, 3),
        /* 0x02 */  PATTERN(0x06, 2),

        // Pattern 0
        /* 0x03 */  NOTE(0x05, 2, 0x3),
        /* 0x04 */  NOTE(0x15, 1, 0x9),
        /* 0x05 */  NOTE(0x10, 0, 0xB),

        // Pattern 1
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

TEST_CASE_METHOD(Fixture, "pattern: Single pattern repeat", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x01, 1, 0x01),
        /* 0x01 */  PATTERN(0x02, 2),

        // Pattern 0
        /* 0x02 */  NOTE(05, 2, 3),
        /* 0x03 */  NOTE(15, 1, 9),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(100);

    bench.tick(100);

    CHECK(allNotes(50) == Vector8({05, 15, 05, 15, 05, 15, 05, 15, 5, 15}));
}

TEST_CASE_METHOD(Fixture, "pattern: Multi pattern repeat", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x03, 1, 0x02),

        /* 0x01 */  PATTERN(0x04, 3),
        /* 0x02 */  PATTERN(0x07, 2),
        /* 0x03 */  PATTERN(0x09, 1),

        // Pattern 0
        /* 0x04 */  NOTE(0x05, 2, 0x3),
        /* 0x05 */  NOTE(0x15, 1, 0x9),
        /* 0x06 */  NOTE(0x10, 0, 0xB),

        // Pattern 1
        /* 0x07 */  NOTE(0x08, 1, 0x1),
        /* 0x08 */  NOTE(0x04, 2, 0x2),

        // Pattern 2
        /* 0x09 */  NOTE(0x03, 10, 0x4),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(125);

    bench.tick(125);

    CHECK(allNotes(125) == Vector8({
        0x05, 0x15, 0x10,
        0x08, 0x04,
        0x03,
        0x08, 0x04,
        0x03,
        0x08, 0x04,
        0x03,
    }));
}

TEST_CASE_METHOD(Fixture, "pattern: Scale song", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  0x1000,
        /* 0x01 */  0x0802,
        /* 0x02 */  0x00CC,
        /* 0x03 */  0x00CE,
        /* 0x04 */  0x00D0,
        /* 0x05 */  0x00D1,
        /* 0x06 */  0x00D3,
        /* 0x07 */  0x00D5,
        /* 0x08 */  0x00D7,
        /* 0x09 */  0x00D8,
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(250);

    bench.tick(250);
}

TEST_CASE_METHOD(Fixture, "pattern: Scale song 2", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x01, 1, 0x01),
        /* 0x01 */  PATTERN(0x02, 8),

        /* 0x02 */  NOTE(0x18, 4, 0),
        /* 0x03 */  NOTE(0x1C, 4, 0),
        /* 0x04 */  NOTE(0x20, 4, 0),
        /* 0x05 */  NOTE(0x22, 4, 0),
        /* 0x06 */  NOTE(0x18, 4, 0),
        /* 0x07 */  NOTE(0x18, 4, 0),
        /* 0x08 */  NOTE(0x18, 4, 0),
        /* 0x09 */  NOTE(0x18, 4, 0),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(250);

    bench.tick(250);
}

TEST_CASE_METHOD(Fixture, "pattern: Scale song 3", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x01, 1, 0x01),
        /* 0x01 */  PATTERN(0x02, 8),

        /* 0x02 */  NOTE(0x0C, 4, 0),
        /* 0x03 */  NOTE(0x0E, 4, 0),
        /* 0x04 */  NOTE(0x10, 4, 0),
        /* 0x05 */  NOTE(0x12, 4, 0),
        /* 0x06 */  NOTE(0x13, 4, 0),
        /* 0x07 */  NOTE(0x15, 4, 0),
        /* 0x08 */  NOTE(0x17, 4, 0),
        /* 0x09 */  NOTE(0x18, 4, 0),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(250);

    bench.tick(250);
}


TEST_CASE_METHOD(Fixture, "pattern: Scale song 4", "[pattern-seq]")
{
    static uint16_t ROM[] = {
        /* 0x00 */  HEADER(0x02, 0, 0x02),
        /* 0x01 */  PATTERN(0x03, 4),
        /* 0x02 */  PATTERN(0x07, 4),

        /* 0x03 */  NOTE(0x0C, 3, 0),
        /* 0x04 */  NOTE(0x0E, 3, 0),
        /* 0x05 */  NOTE(0x10, 3, 0),
        /* 0x06 */  NOTE(0x11, 7, 0),

        /* 0x07 */  NOTE(0x13, 3, 0),
        /* 0x08 */  NOTE(0x15, 3, 0),
        /* 0x09 */  NOTE(0x17, 3, 0),
        /* 0x0A */  NOTE(0x18, 7, 0),
    };
    memcpy(core.zz_memory, ROM, sizeof(ROM));
    setupNoteStrobe(250);
    printf("---- notes: \n");
    for (int i = 0; i < sizeof(ROM)/sizeof(ROM[0]); i++) {
        printf("%04X\n", ROM[i]);
    }
    printf("----\n");

    bench.tick(250);
}

