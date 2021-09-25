#include "test-common.hpp"
#include "Vnote_sequencer_tb.h"

struct Vnote_sequencer_adapter : public Vnote_sequencer_tb
{
    void setClock(uint64_t clock) { i_clk = clock; }
};

using UUT = Vnote_sequencer_adapter;

struct NoteSequencerFixture : TestFixture<UUT>
{
    NoteSequencerFixture()
    {
    }
};

using Fixture = NoteSequencerFixture;

TEST_CASE_METHOD(Fixture, "Sequencer initial state", "[seq]")
{
}

TEST_CASE_METHOD(Fixture, "Test 1", "[seq]")
{
}
