#include "test-common.hpp"
#include "Vduration_counter.h"

void setClock(Vduration_counter& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vduration_counter;

#define MakeInput(_I_) #_I_(makeInput(&UUT:: #_I_))

struct DurationCounterFixture : TestFixture<UUT>
{
    Input8 i_rst, i_enable, i_load, i_duration;
    Output8 o_done;
    DurationCounterFixture() :
        i_rst(makeInput(&UUT::i_rst)),
        i_enable(makeInput(&UUT::i_enable)),
        i_load(makeInput(&UUT::i_load)),
        i_duration(makeInput(&UUT::i_duration)),
        o_done(makeOutput(&UUT::o_done))
    {
    }

};

using Fixture = DurationCounterFixture;

TEST_CASE_METHOD(Fixture, "Is not running when initialized", "[duration-counter]")
{
    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "Initialized to not running", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "Zero duration", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 0}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{5, 1}, {6, 0}}));
}

TEST_CASE_METHOD(Fixture, "Runs when loaded", "[duration-counter]")
{
    i_enable.addInputs({{1, 1}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{7, 1}, {8, 0}}));
}

TEST_CASE_METHOD(Fixture, "Runs only when enabled", "[duration-counter]")
{
    // i_enable.addInputs({{10, 1}, {12, 0}, {15, 1}, {20, 0}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8());
    // CHECK(o_done.changes() == ChangeVector8({{7, 1}, {8, 0}}));
}

TEST_CASE_METHOD(Fixture, "Runs when partially enabled", "[duration-counter]")
{
    i_enable.addInputs({{10, 1}, {12, 0}, {15, 1}, {20, 0}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{15, 1}, {16, 0}}));
}

TEST_CASE_METHOD(Fixture, "Runs when enabled every other clock", "[duration-counter]")
{
    i_enable.addInputs({{4, 1}, {5, 0}, {6, 1}, {7, 0}, {8, 1}, {9, 0}, {10, 1}, {11, 0}});
    i_load.addInputs({{4, 1}, {5, 0}});
    i_duration.addInputs({{4, 2}, {5, 0}});

    bench.tick(25);

    CHECK(o_done.changes() == ChangeVector8({{10, 1}, {11, 0}}));
}
