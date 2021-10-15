#include "test-common.hpp"
#include "Vnote_player_tb.h"

void setClock(Vnote_player_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vnote_player_tb;

#define MakeInput(_I_) #_I_(makeInput(&UUT:: #_I_))

struct NotePlayerFixture : TestFixture<UUT>
{
    Input8 i_frame_stb;
    Input8 i_pitch, i_duration, i_instrument;
    Output32 o_phase_delta;
    NotePlayerFixture() :
        // MakeInput(i_frame_stb),
        i_frame_stb(makeInput(&UUT:: i_frame_stb)),
        i_pitch(makeInput(&UUT::i_pitch)),
        i_duration(makeInput(&UUT::i_duration)),
        i_instrument(makeInput(&UUT::i_instrument)),
        o_phase_delta(makeOutput(&UUT::o_phase_delta))
    {
        uint16_t rom[256];
        memset(rom, 0, sizeof(rom));

        rom[0x00] = 0x0000;     // Pitch 0
        rom[0x01] = 0xFFA5;

        rom[0x02] = 0x0001;     // Pitch 1
        rom[0x03] = 0xFFA5;

        rom[0x04] = 0x0002;     // Pitch 2
        rom[0x05] = 0xFFA5;

        rom[0x80] = 0x0123;     // Instrument 0-3 Lengths
        rom[0x81] = 0x4567;     // Instrument 4-7 Lengths

        rom[0x90] = 0xFEDC;     // Instrument 3 Envelope
        rom[0x91] = 0xBA98;
        rom[0x92] = 0x7654;
        rom[0x93] = 0x3210;

        memcpy(core.zz_memory, rom, sizeof(rom));
    }

    void setupFrameStrobe(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_frame_stb.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }
};

using Fixture = NotePlayerFixture;

TEST_CASE_METHOD(Fixture, "Play one note", "[note-player]")
{
    i_pitch.addInput({3, 1});
    i_duration.addInput({3, 5});
    i_instrument.addInput({3, 3});
    setupFrameStrobe(75);

    bench.tick(75);
}
