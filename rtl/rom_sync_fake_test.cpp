#include "test-common.hpp"
#include "Vrom_sync_fake.h"

struct Vrom_sync_fake_adapter : public Vrom_sync_fake
{
    void setClock(uint64_t clock) { clk = clock; }
};

using UUT = Vrom_sync_fake_adapter;

struct RomSyncFakeFixture : TestFixture<UUT>
{
    Input8 addr;
    Output16 data;
    RomSyncFakeFixture() :
        addr(makeInput(&UUT::addr)),
        data(makeOutput(&UUT::data))
    {
        static uint16_t ROM[] = {
            16, 15, 14, 23, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };
        memcpy(core.memory, ROM, sizeof(ROM));
    }
};

using Fixture = RomSyncFakeFixture;

TEST_CASE_METHOD(Fixture, "Fake synchronous ROM lookup", "[rom]")
{
    bench.openTrace("/tmp/rom_sync_lookup.vcd");
    addr.addInputs({
        {5, 1}, {10, 2}, {12, 3}
    });

    bench.tick(20);

    CHECK(data.changes() == ChangeVector16({
        {1,  16}, {5, 15}, {10, 14}, {12, 23}
    }));
}
