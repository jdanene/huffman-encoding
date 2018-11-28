#include "bit_io.h"
#include <catch.h>
#include <memory>

using namespace ipd;

using bistream_ptr = std::unique_ptr<bistream>;
using bostringstream_ptr = std::unique_ptr<bostringstream>;

using bytes_t = std::vector<uint8_t>;

static bistream_ptr make_bistringstream(std::vector<uint8_t> bytes)
{
    return bistream_ptr(new bistringstream(bytes));
}

TEST_CASE("Read a 0 bit from all-0s bistringstream")
{
    auto b = make_bistringstream({0});
    bool bit;

    b->read(bit);
    CHECK_FALSE(bit);
}

TEST_CASE("Read a 1 bit from all-1s bistringstream")
{
    auto b = make_bistringstream({255});
    bool bit;

    b->read(bit);
    CHECK(bit);
}

TEST_CASE("Read a 1 bit from bistringtream")
{
    auto b = make_bistringstream({128});
    bool bit;

    b->read(bit);
    CHECK(bit);
}

TEST_CASE("Read three bits (two 1s) from bistringstream")
{
    auto b = make_bistringstream({1 << 6 | 1 << 5});
    bool bit;

    b->read(bit);
    CHECK_FALSE(bit);
    b->read(bit);
    CHECK(bit);
    b->read(bit);
    CHECK(bit);
}


TEST_CASE("Read 11 bits from bistringstream")
{
    auto b = make_bistringstream({255, 1 << 6 | 1 << 5});
    bool bit;

    for (int i = 0; i < 8; i++)
    {
        b->read(bit);
    }

    b->read(bit);
    CHECK_FALSE(bit);
    b->read(bit);
    CHECK(bit);
    b->read(bit);
    CHECK(bit);
}


TEST_CASE("Read 16 bits from bistringstream, then check for eof")
{
    auto b = make_bistringstream({255, 255});
    bool bit;

    for (int i = 0; i < 16; i++) {
        b->read(bit);
        CHECK(bit);
    }

    CHECK(b->eof());
}


TEST_CASE("Write a 0 bit to bostringstream")
{
    bostringstream_ptr b(new bostringstream);
    bostream& br = *b;
    CHECK(b->bits_written() == 0);
    br.write(false);
    CHECK(b->bits_written() == 1);
    CHECK(b->data()[0] == 0);
}

TEST_CASE("Write a 1 bit to bostringstream")
{
    bostringstream_ptr b(new bostringstream);
    bostream& br = *b;
    br.write(true);
    CHECK(b->data()[0] == 128);
}

TEST_CASE("Write eight bits to bostringstream")
{
    bostringstream_ptr b(new bostringstream);
    bostream& br = *b;
    br.write_bits('a', 8);
    CHECK(b->bits_written() == 8);
    CHECK(b->data()[0] == 'a');
}
