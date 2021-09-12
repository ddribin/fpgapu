#if __INTELLISENSE__
#pragma diag_suppress 2486
#endif

#include "TestBench.hpp"
#include "SignalPublisher.hpp"
#include "SignalObserver.hpp"
#include "TestFixture.hpp"
#include <verilated.h>

// This must be last so any "operator <<"" overloads work properly for test failures
#include <catch2/catch.hpp>
