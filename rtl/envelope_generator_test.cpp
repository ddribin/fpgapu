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
    memcpy(core.zz_memory, ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {5, 1}, {6, 0} });
    i_instrument.addInputs({ {5, 0} });
    i_strobe.addInputs({ {10, 1}, {11, 0}, {20, 1}, {21, 0} });

    bench.tick(30);

    CHECK(o_valid.changes() == ChangeVector8({ {15, 1}, {16, 0}, {25, 1}, {26, 0} }));
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
    memcpy(core.zz_memory, ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {5, 1}, {6, 0} });
    i_instrument.addInputs({ {5, 0} });
    i_strobe.addInputs({ {10, 1}, {11, 0}, {20, 1}, {21, 0} });

    bench.tick(30);

    CHECK(o_valid.changes() == ChangeVector8({ {15, 1}, {16, 0}, {25, 1}, {26, 0} }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 15} }));
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
    memcpy(core.zz_memory, ROM, sizeof(ROM));

    i_load_instrument.addInputs({ {5, 1}, {6, 0} });
    i_instrument.addInputs({ {5, 0} });
    i_strobe.addInputs({ {10, 1}, {11, 0}, {20, 1}, {21, 0} });

    bench.tick(30);

    CHECK(o_valid.changes() == ChangeVector8({ {15, 1}, {16, 0}, {25, 1}, {26, 0} }));
    CHECK(o_amplitude.changes() == ChangeVector8({ {15, 15} }));
}
