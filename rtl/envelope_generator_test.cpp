#include "test-common.hpp"
#include "Venvelope_generator_tb.h"

void setClock(Venvelope_generator_tb& core, uint8_t clock) {
    core.i_clk = clock;
}

using UUT = Venvelope_generator_tb;

struct EnvelopeGeneratorFixture : TestFixture<UUT>
{
    Input8 i_rst;
    Input8 i_load_instrument, i_instrument, i_strobe;
    Output8 o_valid, o_amplitude;
    EnvelopeGeneratorFixture() :
        MakeInput(i_rst),
        MakeInput(i_load_instrument),
        MakeInput(i_instrument),
        MakeInput(i_strobe),
        MakeOutput(o_valid),
        MakeOutput(o_amplitude)
    {
    }

    void copyToROM(const void *data, size_t len, uint8_t offset = 0x00)
    {
        static const uint8_t BASE = 0x10;
        memcpy(core.zz_memory + BASE + offset, data, len);
    }


    void setupStrobe(uint64_t endTime)
    {
        uint64_t time = 10;
        while (time < endTime) {
            i_strobe.addInputs({{time, 1}, {time+1, 0}});
            time += 10;
        }
    }

};

using Fixture = EnvelopeGeneratorFixture;

#define LENGTHS(LEN0, LEN1, LEN2, LEN3) ((LEN3 << 12) | (LEN2 << 8) | (LEN1 << 4) | LEN0)
#define ENVELOPES(ENV0, ENV1, ENV2, ENV3) ((ENV3 << 12) | (ENV2 << 8) | (ENV1 << 4) | ENV0)

TEST_CASE_METHOD(Fixture, "envelope: instrument length 1", "[envelope-generator]")
{
    static uint16_t ROM[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(0,  0,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  0),

        // Instrument 0 Envelopes
        /* 0x04 */  ENVELOPES(15, 0,  0,  0),
        /* 0x05 */  ENVELOPES(0,  0,  0,  0),
        /* 0x06 */  ENVELOPES(0,  0,  0,  0),
        /* 0x07 */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 0} });
    setupStrobe(40);

    bench.tick(40);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 15} }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument length 2", "[envelope-generator]")
{
    static uint16_t ROM[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(1,  0,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  0),

        // Instrument 0 Envelopes
        /* 0x04 */  ENVELOPES(15, 0,  0,  0),
        /* 0x05 */  ENVELOPES(0,  0,  0,  0),
        /* 0x06 */  ENVELOPES(0,  0,  0,  0),
        /* 0x07 */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 0} });
    setupStrobe(40);

    bench.tick(40);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 15}, {23, 0} }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument length 4", "[envelope-generator]")
{
    static uint16_t ROM[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(3,  0,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  0),

        // Instrument 0 Envelopes
        /* 0x04 */  ENVELOPES(15, 14, 13, 12),
        /* 0x05 */  ENVELOPES(0,  0,  0,  0),
        /* 0x06 */  ENVELOPES(0,  0,  0,  0),
        /* 0x07 */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 0} });
    setupStrobe(60);

    bench.tick(60);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},
        {43, 1}, {44, 0},
        {53, 1}, {54, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 15}, {23, 14}, {33, 13}, {43, 12} }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument 1 length 4", "[envelope-generator]")
{
    static uint16_t ROM[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(2,  3,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  0),

        // Instrument 0 Envelopes
        /* 0x04 */  ENVELOPES(15, 14, 0,  0),
        /* 0x05 */  ENVELOPES(0,  0,  0,  0),
        /* 0x06 */  ENVELOPES(0,  0,  0,  0),
        /* 0x07 */  ENVELOPES(0,  0,  0,  0),

        // Instrument 1 Envelopes
        /* 0x08 */  ENVELOPES(1,  2,  3,  4),
        /* 0x09 */  ENVELOPES(0,  0,  0,  0),
        /* 0x0A */  ENVELOPES(0,  0,  0,  0),
        /* 0x0B */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 1} });
    setupStrobe(60);

    bench.tick(60);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},
        {43, 1}, {44, 0},
        {53, 1}, {54, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 1}, {23, 2}, {33, 3}, {43, 4} }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument F length 4", "[envelope-generator]")
{
    static uint16_t LENGTHS[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(0,  0,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  3),
    };
    copyToROM(LENGTHS, sizeof(LENGTHS));

    static uint16_t ENVELOPES[] = {
        // Instrument F Envelopes
        /* 0x40 */  ENVELOPES(1,  2,  3,  4),
        /* 0x41 */  ENVELOPES(0,  0,  0,  0),
        /* 0x42 */  ENVELOPES(0,  0,  0,  0),
        /* 0x43 */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ENVELOPES, sizeof(ENVELOPES), 0x40);

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 0xF} });
    setupStrobe(60);

    bench.tick(60);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},
        {43, 1}, {44, 0},
        {53, 1}, {54, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 1}, {23, 2}, {33, 3}, {43, 4} }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument F length 15", "[envelope-generator]")
{
    static uint16_t LENGTHS[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(0,  0,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  15),
    };
    copyToROM(LENGTHS, sizeof(LENGTHS));

    static uint16_t ENVELOPES[] = {
        // Instrument F Envelopes
        /* 0x40 */  ENVELOPES( 1,  2,  3,  4),
        /* 0x41 */  ENVELOPES( 5,  6,  7,  8),
        /* 0x42 */  ENVELOPES( 9, 10, 11, 12),
        /* 0x43 */  ENVELOPES(13, 14, 15,  0),
    };
    copyToROM(ENVELOPES, sizeof(ENVELOPES), 0x40);

    i_load_instrument.addInputs({ {10, 1}, {11, 0} });
    i_instrument.addInputs({ {10, 0xF} });
    setupStrobe(200);

    bench.tick(200);

    CHECK(o_valid.changes() == ChangeVector8({
        { 15, 1}, { 16, 0},
        { 23, 1}, { 24, 0},
        { 33, 1}, { 34, 0},
        { 43, 1}, { 44, 0},
        { 53, 1}, { 54, 0},
        { 63, 1}, { 64, 0},
        { 73, 1}, { 74, 0},
        { 83, 1}, { 84, 0},
        { 93, 1}, { 94, 0},

        {103, 1}, {104, 0},
        {113, 1}, {114, 0},
        {123, 1}, {124, 0},
        {133, 1}, {134, 0},
        {143, 1}, {144, 0},
        {153, 1}, {154, 0},
        {163, 1}, {164, 0},
        {173, 1}, {174, 0},
        {183, 1}, {184, 0},
        {193, 1}, {194, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({
        { 15,  1}, { 23,  2}, { 33,  3}, { 43,  4},
        { 53,  5}, { 63,  6}, { 73,  7}, { 83,  8},
        { 93,  9}, {103, 10}, {113, 11}, {123, 12},
        {133, 13}, {143, 14}, {153, 15}, {163,  0},
    }));
}

TEST_CASE_METHOD(Fixture, "envelope: instrument 1 then 0", "[envelope-generator]")
{
    static uint16_t ROM[] = {
        // Instrument Lengths
        /* 0x00 */  LENGTHS(2,  3,  0,  0),
        /* 0x01 */  LENGTHS(0,  0,  0,  0),
        /* 0x02 */  LENGTHS(0,  0,  0,  0),
        /* 0x03 */  LENGTHS(0,  0,  0,  0),

        // Instrument 0 Envelopes
        /* 0x04 */  ENVELOPES(15, 14, 0,  0),
        /* 0x05 */  ENVELOPES(0,  0,  0,  0),
        /* 0x06 */  ENVELOPES(0,  0,  0,  0),
        /* 0x07 */  ENVELOPES(0,  0,  0,  0),

        // Instrument 1 Envelopes
        /* 0x08 */  ENVELOPES(1,  2,  3,  4),
        /* 0x09 */  ENVELOPES(0,  0,  0,  0),
        /* 0x0A */  ENVELOPES(0,  0,  0,  0),
        /* 0x0B */  ENVELOPES(0,  0,  0,  0),
    };
    copyToROM(ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {10, 1}, {11, 0}, {40, 1}, {41, 0} });
    i_instrument.addInputs({ {10, 1}, {40, 0} });
    setupStrobe(80);

    bench.tick(80);

    CHECK(o_valid.changes() == ChangeVector8({
        {15, 1}, {16, 0},
        {23, 1}, {24, 0},
        {33, 1}, {34, 0},

        {45, 1}, {46, 0},
        {53, 1}, {54, 0},
        {63, 1}, {64, 0},
        {73, 1}, {74, 0},
    }));
    CHECK(o_amplitude.changes() == ChangeVector8({
        {15,  1}, {23,  2}, {33,  3},
        {45, 15}, {53, 14}, {63,  0},
    }));
}
