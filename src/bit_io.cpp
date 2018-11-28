#include "bit_io.h"

#include <ios>

namespace ipd {

using std::ios;

bistream& bistream::read(bool& bit)
{
    if (nbits_ == 0) {
        if (!get_next_byte(bitbuf_)) return *this;
        nbits_ = 8;
    }

    --nbits_;
    bit = (bitbuf_ >> nbits_ & 1) == 1;

    return *this;
}

bistream::operator bool() const
{
    return good();
}

bool bistream::eof() const
{
    return nbits_ == 0;
}

bool bistream::good() const
{
    return nbits_ > 0;
}

bistream_adaptor::bistream_adaptor(std::istream& is) : stream_(is)
{ }

bool bistream_adaptor::get_next_byte(uint8_t& bitbuf)
{
    return (bool) stream_.read((char*)&bitbuf, 1);
}

bool bistream_adaptor::good() const
{
    return bistream::good() || stream_.good();
}

bool bistream_adaptor::eof() const
{
    return bistream::eof() && stream_.eof();
}

bistringstream::bistringstream(std::vector<uint8_t> v)
        : bytes_(v), bytes_index_(0)
{ }

static std::vector<uint8_t>
bits_to_bytes(std::initializer_list<bool> bits)
{
    bostringstream oss;
    for (bool bit : bits) oss << bit;
    return oss.data();
}

bistringstream::bistringstream(std::initializer_list<bool> bits)
        : bistringstream(bits_to_bytes(bits))
{ }

bool bistringstream::get_next_byte(uint8_t& bitbuf)
{
    if (bytes_index_ < bytes_.size()) {
        bitbuf = bytes_[bytes_index_];
        bytes_index_++;
        return true;
    }
    bitbuf = 0;
    return false;
}

bool bistringstream::good() const
{
    return bistream::good() || bytes_index_ < bytes_.size();
}

size_t bostringstream::bits_written() const
{
    return bits_written_;
}

bool bistringstream::eof() const
{
    return bistream::eof() && bytes_index_ == bytes_.size();
}

bifstream::bifstream(const char* filespec)
        : base_(filespec, ios::binary)
        , bistream_adaptor(base_)
{}

bostream_adaptor::bostream_adaptor(std::ostream& os)
        : stream_(os)
{}

bostream_adaptor& bostream_adaptor::write(bool bit)
{
    bitbuf_ |= ((uint8_t) bit) << (7 - nbits_);

    if (++nbits_ == 8) write_out_();

    return *this;
}

bool bostream_adaptor::good() const
{
    return stream_.good();
}

void bostream_adaptor::write_out_()
{
    if (stream_.write((char*)&bitbuf_, 1)) {
        bitbuf_ = 0;
        nbits_ = 0;
    }
}

bostream_adaptor::~bostream_adaptor()
{
    if (nbits_) {
        write_out_();
    }
}

bostream::operator bool() const
{
    return good();
}

bofstream::bofstream(const char* filespec)
        : base_(filespec, ios::binary | ios::trunc)
        , bostream_adaptor(base_)
{}

bool bostringstream::good() const
{
    return true;
}

bostringstream& bostringstream::write(bool bit)
{
    auto index = bits_written_ / 8;
    auto nbits = bits_written_ % 8;
    if (index >= data_.size()) data_.push_back(0);
    data_[index] |= ((uint8_t) bit) << (7 - nbits);
    bits_written_++;
    return *this;
}

std::vector<uint8_t> const& bostringstream::data() const
{
    return data_;
}

} // end namespace ipd

