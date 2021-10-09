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

#endif
