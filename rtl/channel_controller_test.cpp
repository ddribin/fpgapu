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
    ChannelControllerFixture() :
        MakeInput(i_rst),
        MakeInput(i_tick_stb),
        MakeInput(i_note_stb)
    {
    }

};

using Fixture = ChannelControllerFixture;

TEST_CASE_METHOD(Fixture, "channel: Is not running when initialized", "[channel]")
{
    i_tick_stb.addInputs({
        {4,  1}, {5,  0}, {9,  1}, {10, 0}, {14, 1}, {15, 0}, {19, 1}, {20, 0},
        {24, 1}, {25, 0},
    });
    i_note_stb.addInputs({ {4, 1}, {5, 0}, {24, 1}, {25, 0}, });

    bench.tick(30);
}
