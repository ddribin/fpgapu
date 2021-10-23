#include "test-common.hpp"
#include "Vchannel_controller.h"

void setClock(Vchannel_controller& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vchannel_controller;

#define MakeInput(_I_) _I_(makeInput(&UUT:: _I_))
#define MakeOutput(_I_) _I_(makeOutput(&UUT:: _I_))

struct ChannelControllerFixture : TestFixture<UUT>
{
    Input8 i_rst, i_tick_stb, i_note_stb;
    Input8 i_pattern_valid, i_pitch_lookup_valid;
    ChannelControllerFixture() :
        MakeInput(i_rst),
        MakeInput(i_tick_stb),
        MakeInput(i_note_stb),
        MakeInput(i_pattern_valid),
        MakeInput(i_pitch_lookup_valid)
    {
    }

    template<class Input>
    void setupPeriodicPulse(Input &input, uint64_t startTime, uint64_t endTime, uint64_t period)
    {
        uint64_t time = startTime;
        while (time < endTime) {
            input.addInputs({{time, 1}, {time+1, 0}});
            time += period;
        }
    }

};

using Fixture = ChannelControllerFixture;

TEST_CASE_METHOD(Fixture, "channel: Is not running when initialized", "[channel]")
{
    setupPeriodicPulse(i_tick_stb, 5, 100, 10);
    setupPeriodicPulse(i_note_stb, 5, 100, 20);
    i_pattern_valid.addInputs({ {7, 1}, {8, 0} });
    i_pitch_lookup_valid.addInputs({ {9, 1}, {10, 0} });

    bench.tick(100);
}
