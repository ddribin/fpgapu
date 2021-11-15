#ifndef	TEST_FIXTURE_H
#define	TEST_FIXTURE_H

#include "verilatest.h"
#include "test-common.hpp"

template<class Core>
struct TestFixture : TestFixtureBase<Core> {
    TestFixture() {
        this->bench.openTrace(vcdNameForCurrentTest().c_str());
    }
};

#define MakeInput(_I_) _I_(makeInput(&UUT::_I_))
#define MakeOutput(_O_) _O_(makeOutput(&UUT::_O_))

#endif
