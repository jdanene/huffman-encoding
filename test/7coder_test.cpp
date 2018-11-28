#include "7coder.h"
#include <catch.h>

#include <sstream>

using namespace std;
using namespace ipd;

static void round_trip(string const& msg)
{
    istringstream is(msg);
    bostringstream boss;
    encode(is, boss);

    bistringstream biss(boss.data());
    ostringstream os;
    decode(biss, os);

    CHECK(os.str() == msg);
}

TEST_CASE("Round trip empty string")
{
    round_trip("");
}

TEST_CASE("Round trip A")
{
    round_trip("A");
}

TEST_CASE("Round trip hello")
{
    round_trip("hello");
}

TEST_CASE("Round trip Hello world")
{
    round_trip("Hello, world!");
}
