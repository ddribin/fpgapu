#include "test-common.hpp"
#include "Vnote_player_tb.h"

void setClock(Vnote_player_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vnote_player_tb;

#define MakeInput(_I_) #_I_(makeInput(&UUT:: #_I_))

struct NotePlayerFixture : TestFixture<UUT>
{
    Input8 i_tick;
    Input8 i_pitch, i_duration, i_instrument;
    Output32 o_phase_delta;
    NotePlayerFixture() :
        // MakeInput(i_tick),
        i_tick(makeInput(&UUT:: i_tick)),
        i_pitch(makeInput(&UUT::i_pitch)),
        i_duration(makeInput(&UUT::i_duration)),
        i_instrument(makeInput(&UUT::i_instrument)),
        o_phase_delta(makeOutput(&UUT::o_phase_delta))
    {
        uint16_t rom[256];
        memset(rom, 0, sizeof(rom));

        rom[0x00] = 0x0000;     // Note 0
        rom[0x01] = 0xFFA5;

        rom[0x02] = 0x0001;     // Note 1
        rom[0x03] = 0xFFA5;

        rom[0x04] = 0x0002;     // Note 2
        rom[0x05] = 0xFFA5;

        rom[0x81] = 0x1234;
        rom[0x8E] = 0x5678;

        memcpy(core.zz_memory, rom, sizeof(rom));
    }

    void setupNoteStrobe(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_tick.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }
};

using Fixture = NotePlayerFixture;

TEST_CASE_METHOD(Fixture, "Play one note", "[note-player]")
{
    i_pitch.addInput({3, 2});
    i_instrument.addInput({3, 5});
    setupNoteStrobe(75);

    bench.tick(75);
}
