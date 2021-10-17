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
    Input8 i_rst;
    ChannelControllerFixture() :
        MakeInput(i_rst)
    {
    }

};

using Fixture = ChannelControllerFixture;

TEST_CASE_METHOD(Fixture, "channel: Is not running when initialized", "[channel]")
{
    bench.tick(25);
}
