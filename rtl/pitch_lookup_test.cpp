#include "test-common.hpp"
#include "Vpitch_lookup_tb.h"

void setClock(Vpitch_lookup_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpitch_lookup_tb;

#define MakeInput(_I_) _I_(makeInput(&UUT:: _I_))
#define MakeOutput(_I_) _I_(makeOutput(&UUT:: _I_))

struct PitchLookupFixture : TestFixture<UUT>
{
    Input8 i_rst, i_enable, i_pitch;
    Output32 o_phase_delta;
    Output8 o_valid;
    PitchLookupFixture() :
        MakeInput(i_rst),
        MakeInput(i_enable),
        MakeInput(i_pitch),
        MakeOutput(o_phase_delta),
        MakeOutput(o_valid)
    {
        uint16_t rom[256];
        memset(rom, 0, sizeof(rom));

        rom[0x00] = 0x0000;     // Pitch 0
        rom[0x01] = 0xFFA5;

        rom[0x02] = 0x0001;     // Pitch 1
        rom[0x03] = 0xFFA5;

        rom[0x04] = 0x0002;     // Pitch 2
        rom[0x05] = 0xFFA5;

        memcpy(core.memory, rom, sizeof(rom));
    }

    void setupEnable(uint64_t endTime)
    {
        uint64_t time = 5;
        while (time < endTime) {
            i_enable.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }
};

using Fixture = PitchLookupFixture;

TEST_CASE_METHOD(Fixture, "pitch: Lookup first note", "[pitch]")
{
    i_enable.addInputs({ {4, 1}, {5, 0} });
    i_pitch.addInputs({{4, 0}});

    bench.tick(10);

    CHECK(o_valid.changes() == ChangeVector8({ {8, 1}, {9, 0} }));
    CHECK(o_phase_delta.changes() == ChangeVector32({ {8, 0xFFA50000} }));
}

TEST_CASE_METHOD(Fixture, "pitch: Lookup second note", "[pitch]")
{
    i_enable.addInputs({{4, 1}, {5, 0}});
    i_pitch.addInputs({ {4, 1} });

    bench.tick(10);

    CHECK(o_valid.changes() == ChangeVector8({ {8, 1}, {9, 0} }));
    CHECK(o_phase_delta.timeline(10)[8] == 0xFFA50001);
}

TEST_CASE_METHOD(Fixture, "pitch: Lookup three notes", "[pitch]")
{
    i_enable.addInputs({
        {4,  1}, {5,  0},
        {9,  1}, {10, 0},
        {14, 1}, {15, 0},
    });
    i_pitch.addInputs({ {4, 1}, {9, 0}, {14, 2} });

    bench.tick(20);

    CHECK(o_valid.changes() == ChangeVector8({
        {8,  1}, {9,  0},
        {13, 1}, {14, 0},
        {18, 1}, {19, 0},
    }));
    Vector32 o_phase_delta_timeline = o_phase_delta.timeline(20);
    CHECK(o_phase_delta_timeline[8]  == 0xFFA50001);
    CHECK(o_phase_delta_timeline[13] == 0xFFA50000);
    CHECK(o_phase_delta_timeline[18] == 0xFFA50002);
}

TEST_CASE_METHOD(Fixture, "pitch: Handles reset", "[pitch]")
{
    i_enable.addInputs({ {4, 1}, {5, 0} });
    i_pitch.addInputs({ {4, 0} });
    i_rst.addInputs({ {7, 1}, {8, 0} });

    bench.tick(10);

    CHECK(o_valid.changes() == ChangeVector8());
    CHECK(o_phase_delta.changes() == ChangeVector32());
}
