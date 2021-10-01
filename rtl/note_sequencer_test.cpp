#include "test-common.hpp"
#include "Vnote_sequencer_tb.h"

struct Vnote_sequencer_adapter : public Vnote_sequencer_tb
{
    void setClock(uint64_t clock) { i_clk = clock; }
};

using UUT = Vnote_sequencer_adapter;

#define NOTE(NOTE, LEN, INSTRUMENT) (NOTE | (LEN << 6) | (INSTRUMENT << 11))

struct NoteSequencerFixture : TestFixture<UUT>
{
    Input8 i_note_stb;
    Input8 i_new_addr;
    Input8 i_new_pattern_len;
    Input8 i_new_addr_valid;
    NoteSequencerFixture() :
        i_note_stb(makeInput(&UUT::i_note_stb_tb)),
        i_new_addr(makeInput(&UUT::i_new_addr)),
        i_new_pattern_len(makeInput(&UUT::i_new_pattern_len)),
        i_new_addr_valid(makeInput(&UUT::i_new_addr_valid))
    {
        static uint16_t ROM[15] = {
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
            NOTE(0x05, 2, 0x3), NOTE(0x15, 1, 0x9), NOTE(0x10, 0, 0xB),
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

    void setupNewPattern(uint8_t addr, uint8_t len, uint64_t time)
    {
        i_new_addr.addInputs({{time, addr}});
        i_new_pattern_len.addInputs({{time, len}});
        i_new_addr_valid.addInputs({{time, 1}, {time+1, 0}});
    }
};

using Fixture = NoteSequencerFixture;

TEST_CASE_METHOD(Fixture, "Sequencer Test", "[seq]")
{
    bench.openTrace("/tmp/sequencer_test.vcd");
    setupNoteStrobe(100);
    setupNewPattern(0, 15, 5);

    bench.tick(103);
}

TEST_CASE_METHOD(Fixture, "Sequencer Test 2", "[seq]")
{
    bench.openTrace("/tmp/sequencer_test_2.vcd");
    setupNoteStrobe(100);
    // setupNewPattern(3, 2, 5);
    
    bench.tick(103);
}
