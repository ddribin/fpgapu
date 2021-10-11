#include "test-common.hpp"
#include "Vpattern_sequencer_tb.h"

void setClock(Vpattern_sequencer_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpattern_sequencer_tb;

#define NOTE(NOTE, LEN, INSTRUMENT) (NOTE | (LEN << 6) | (INSTRUMENT << 11))

struct PatternSequencerFixture : TestFixture<UUT>
{
    Input8 i_note_stb;
    PatternSequencerFixture() :
        i_note_stb(makeInput(&UUT::i_note_stb))
    {
        static uint16_t ROM[8] = {
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9)
        };
        memcpy(core.zz_memory, ROM, sizeof(ROM));
    }

    void setupNoteStrobe(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_note_stb.addInputs({{time, 1}, {time+1, 0}});
            time += 5;
        }
    }
};

using Fixture = PatternSequencerFixture;

TEST_CASE_METHOD(Fixture, "Pattern sequencer initial outputs", "[pattern-seq]")
{
    setupNoteStrobe(100);

    bench.tick(103);
}
