#include "test-common.hpp"
#include "Vpwm.h"

void setClock(Vpwm& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Vpwm;

struct PwmFixture : TestFixture<UUT>
{
    Input8 top, topValid;
    Input8 compare, compareValid;
    Output8 pwm;
    PwmFixture() : top(makeInput(&UUT::i_top)),
                   topValid(makeInput(&UUT::i_top_valid)),
                   compare(makeInput(&UUT::i_compare)),
                   compareValid(makeInput(&UUT::i_compare_valid)),
                   pwm(makeOutput(&UUT::o_pwm))
    {
    }

    void setupTop(uint8_t top_, uint64_t time_ = 1)
    {
        top.addInputs({{time_, top_}, {time_+1, 0}});
        topValid.addInputs({{time_, 1}, {time_+1, 0}});
    }

    void setupCompare(uint8_t compare_, uint64_t time_ = 1)
    {
        compare.addInputs({{time_, compare_}, {time_+1, 0}});
        compareValid.addInputs({{time_, 1}, {time_+1, 0}});
    }
};

using Fixture = PwmFixture;

TEST_CASE_METHOD(Fixture, "PWM: Initial state", "[pwm]")
{
    REQUIRE(core.o_pwm == 0);
}

TEST_CASE_METHOD(Fixture, "PWM: Test no compare set", "[pwm]")
{
    bench.tick(32);
    CHECK(pwm.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "PWM: Test 0/16", "[pwm]")
{
    setupTop(15);
    setupCompare(0);

    bench.tick(35);
    CHECK(pwm.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "PWM: Test 1/16", "[pwm]")
{
    setupTop(15);
    setupCompare(1);

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {3,  0},
        {18, 1}, {19, 0},
        {34, 1}, {35, 0},
    }));
}

TEST_CASE_METHOD(Fixture, "PWM: Test 15/16", "[pwm]")
{
    setupTop(15);
    setupCompare(15);

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {17, 0},
        {18, 1}, {33, 0},
        {34, 1},
    }));
}

TEST_CASE_METHOD(Fixture, "PWM: Test 16/16", "[pwm]")
{
    setupTop(15);
    setupCompare(16);

    bench.tick(35);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1},
    }));
}

// ---

TEST_CASE_METHOD(Fixture, "PWM: Test 0/8", "[pwm]")
{
    setupTop(7);
    setupCompare(0);

    bench.tick(25);
    CHECK(pwm.changes() == ChangeVector8());
}

TEST_CASE_METHOD(Fixture, "PWM: Test 4/8", "[pwm]")
{
    setupTop(7);
    setupCompare(4);

    bench.tick(25);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {6,  0},
        {10, 1}, {14, 0},
        {18, 1}, {22, 0},
    }));
}

TEST_CASE_METHOD(Fixture, "PWM: Test 8/8", "[pwm]")
{
    setupTop(7);
    setupCompare(8);

    bench.tick(25);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1},
    }));
}

// ---

TEST_CASE_METHOD(Fixture, "PWM: Test updating compare up while running", "[pwm]")
{
    setupTop(15);
    setupCompare(1);
    setupCompare(4, 5); // 5 is the middle of the first cycle

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {3,  0},
        {18, 1}, {22, 0},
        {34, 1}, {38, 0},
    }));
}

TEST_CASE_METHOD(Fixture, "PMW: Test updating compare down while running", "[pwm]")
{
    setupTop(15);
    setupCompare(4);
    setupCompare(1, 5); // 5 is the middle of the first high cycle

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {6,  0},
        {18, 1}, {19, 0},
        {34, 1}, {35, 0},
    }));
}

TEST_CASE_METHOD(Fixture, "PWM: Test updating compare on last clock of first cycle", "[pwm]")
{
    setupTop(15);
    setupCompare(4);
    setupCompare(1, 17); // 17 is the last clock of the first cycle

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {6,  0},
        {18, 1}, {19, 0},
        {34, 1}, {35, 0},
    }));
}

TEST_CASE_METHOD(Fixture, "PWM: Test updating compare on first clock of second cycle", "[pwm]")
{
    setupTop(15);
    setupCompare(4);
    setupCompare(1, 18); // 18 is the first clock of the second cycle

    bench.tick(40);
    CHECK(pwm.changes() == ChangeVector8({
        {2,  1}, {6,  0},
        {18, 1}, {22, 0},
        {34, 1}, {35, 0},
    }));
}
