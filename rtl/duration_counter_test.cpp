#include "test-common.hpp"
#include "Vduration_counter.h"

void setClock(Vduration_counter& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vduration_counter;

#define MakeInput(_I_) _I_(makeInput(&UUT:: _I_))
#define MakeOutput(_O_) _O_(makeOutput(&UUT:: _O_))

struct DurationCounterFixture : TestFixture<UUT>
{
    Input8 i_rst, i_enable, i_load, i_duration;
    Output8 o_done;
    DurationCounterFixture() :
        MakeInput(i_rst),
        MakeInput(i_enable),
        MakeInput(i_load),
        MakeInput(i_duration),
        MakeOutput(o_done)
    {
    }

};

using Fixture = DurationCounterFixture;

TEST_CASE_METHOD(Fixture, "duration: Is not running when initialized", "[duration-counter]")
{
    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "duration: Initialized to not running", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "duration: Zero duration", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 0}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{5, 1}, {6, 0}}));
}

TEST_CASE_METHOD(Fixture, "duration: Runs when loaded", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{7, 1}, {8, 0}}));
}

TEST_CASE_METHOD(Fixture, "duration: Runs only when enabled", "[duration-counter]")
{
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "duration: Runs when partially enabled", "[duration-counter]")
{
    i_enable.addInputs({{4, 1}, {6, 0}, {15, 1}, {20, 0}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{16, 1}, {17, 0}}));
}

TEST_CASE_METHOD(Fixture, "duration: Runs when enabled every other clock", "[duration-counter]")
{
    i_enable.addInputs({{4, 1}, {5, 0}, {6, 1}, {7, 0}, {8, 1}, {9, 0}, {10, 1}, {11, 0}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{10, 1}, {11, 0}}));
}

TEST_CASE_METHOD(Fixture, "duration: Not loaded when disabled", "[duration-counter]")
{
    i_enable.addInputs({{6, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "duration: Reset stops running", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});
    i_rst.addInputs({{6, 1}, {7, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}
