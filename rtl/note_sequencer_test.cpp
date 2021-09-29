#include "test-common.hpp"
#include "Vnote_sequencer_tb.h"

struct Vnote_sequencer_adapter : public Vnote_sequencer_tb
{
    void setClock(uint64_t clock) { i_clk = clock; }
};

using UUT = Vnote_sequencer_adapter;

#define NOTE(NOTE, LEN) (NOTE | (LEN << 6))

struct NoteSequencerFixture : TestFixture<UUT>
{
    Input8 i_note_stb;
    NoteSequencerFixture() :
        i_note_stb(makeInput(&UUT::i_note_stb))
    {
        static uint16_t ROM[15] = {
            NOTE(0x05, 2), NOTE(0x15, 1), NOTE(0x10, 0),
            NOTE(0x05, 2), NOTE(0x15, 1), NOTE(0x10, 0),
            NOTE(0x05, 2), NOTE(0x15, 1), NOTE(0x10, 0),
            NOTE(0x05, 2), NOTE(0x15, 1), NOTE(0x10, 0),
            NOTE(0x05, 2), NOTE(0x15, 1), NOTE(0x10, 0),
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

using Fixture = NoteSequencerFixture;

TEST_CASE_METHOD(Fixture, "Sequencer Test", "[seq]")
{
    bench.openTrace("/tmp/sequencer_test.vcd");
    setupNoteStrobe(100);
    bench.tick(103);
}
