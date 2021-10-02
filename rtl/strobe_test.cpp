#include "test-common.hpp"
#include "Vstrobe.h"

struct Vstrobe_adapter : public Vstrobe
{
    void setClock(uint64_t clock) { clk = clock; }
};

using UUT = Vstrobe_adapter;


struct StrobeFixture : TestFixture<UUT>
{
    Input8 reset;
    Input8 strobe_in;
    StrobeFixture() :
        reset(makeInput(&UUT::reset)), 
        strobe_in(makeInput(&UUT::strobe_in))
    {
    }

};

using Fixture = StrobeFixture;

TEST_CASE_METHOD(Fixture, "Strobe Test", "[seq]")
{
    bench.openTrace("/tmp/strobe.vcd");
    reset.addInputs({{1, 1}, {2, 0}});
    strobe_in.addInputs({
        {5, 1},
        {6, 0},
    });

    bench.tick(10);
}
