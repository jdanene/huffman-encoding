#pragma once

/*
 * bit_io.h: classes for reading and writing files one bit (or more)
 * at a time.
 *
 * The main idea is that we create a bit input or output stream as a
 * layer over a file that we open for reading or writing. The library
 * maintains a buffer of bits so that the client can view the file as a
 * simple sequence of bits rather than bytes.
 */

#include <cstdint>
#include <istream>
#include <ostream>
#include <fstream>
#include <vector>
#include <initializer_list>

namespace ipd {
/*
 * INPUT
 */

// Bit input stream, for reading individual bits from a file or a
// std::vector<uint8_t> This is an abstract base class for the concrete
// derived classes `bistream_adaptor`, `bifstream`, and `bistreamstream`
// defined below. It defines the common interface for those classes.
class bistream {
public:
    // Reads a bit from this bit input stream into the given bool
    // reference.
    //
    // Parameters:
    //
    //      result - bool reference to store the bit that was read
    //
    // Returns: the same bit input stream, for method chaining
    //
    // Example:
    //
    //      bool b;
    //      bif.read(b);
    //
    bistream& read(bool& result);

    // Reads `n` bits, interpreted as a big-endian `n`-bit integer,
    // and stores them in the reference `result`, which must have a
    // numeric type.
    //
    // Parameters:
    //
    //      <typename T> - the type of the result; must be numeric
    //      result - reference for storing the result
    //      n - number of bits to read
    //
    // Returns: the same bit input stream, for method chaining
    //
    // Example:
    //
    //      int result;
    //      bif.read_bits(result, 5); // reads 5 bits into result
    //
    template<typename T>
    bistream& read_bits(T& result, size_t n);

    // Determines whether we've reached the end of the input file.
    //
    // Returns: `true` if we’ve attempted to read past the end of
    // the file, and `false` otherwise.
    //
    // Example:
    //
    //      while (bif.read(b) && !bif.eof()) {
    //          ...
    //      }
    //
    virtual bool eof() const;

    // Determines the status of the bit input stream.
    //
    // Returns: `false` if there are no bits to read and a read
    // error has occurred, and `true` otherwise.
    //
    // Example:
    //
    //      bif.read(b);
    //      if (bif.good()) {
    //          // we know the read succeeded
    //      }
    //
    virtual bool good() const;

    // The bit input stream boolean coercion operator, inserted, for
    // example, when a `bifstream` is used as a condition for an
    // `if`. Alias for `good()`.
    //
    // Returns: `false` if there are no bits to read and a read
    // error has occurred, and `true` otherwise.
    //
    // Example:
    //
    //      bif.read(b);
    //      if (bif) {
    //          // we know the read succeeded
    //      }
    //
    operator bool() const;

    virtual ~bistream() { }

protected:
    virtual bool get_next_byte(uint8_t&) = 0;

private:
    uint_fast8_t nbits_ = 0;
    uint8_t bitbuf_ = 0;

};

// Adapts any `std::istream&` for use as a bit input stream.
class bistream_adaptor : public bistream {
public:
    // Constructs a bit input stream on top of an input stream. Does not
    // take ownership of the `std::istream`. However, using the `istream`
    // while also using the adaptor, or adapting the same `istream`
    // twice, will give strange results.
    //
    // Parameters:
    //
    //      input_stream - the input stream to adapt
    //
    // Example:
    //
    //      bistream_adaptor bis(input_stream);
    //
    explicit bistream_adaptor(std::istream&);

    bool eof() const override;

    bool good() const override;

    bistream_adaptor(bistream_adaptor const&) = delete;

private:
    std::istream& stream_;

    virtual bool get_next_byte(uint8_t&) override;
};

class bifstream : public bistream_adaptor {
public:
    // Constructs a bit input stream to read from the given file.
    //
    // Parameters:
    //
    //      filespec - name of the file to open
    //
    // Example:
    //
    //      bifstream bif(input_file_name);
    //
    explicit bifstream(const char* filespec);

    bifstream(bifstream const&) = delete;

private:
    std::ifstream base_;
};

class bistringstream : public bistream {
public:
    // Constructs a bit input stream from the given vector of bytes.
    //
    // Parameters:
    //
    //      bytes - the vector of bytes
    //
    // Example:
    //
    //      std::vector<uint8_t> v{255};
    //      bifstream bif(v);
    //
    explicit bistringstream(std::vector<uint8_t>);

    // Creates a bit input stream containing exactly the given bits.
    explicit bistringstream(std::initializer_list<bool>);

    bool eof() const override;

    bool good() const override;

    bistringstream(bistringstream const&) = delete;

private:
    size_t bytes_index_;
    std::vector<uint8_t> bytes_;

    virtual bool get_next_byte(uint8_t&) override;

};

// The bit input stream extraction operator; alias for `read(bool&)`.
//
// Params:
//
//     - bif - the bit input stream to read from
//     - b - reference to store the bit that was read
//
// Returns: the same bit input stream, for method chaining
//
// Example:
//
//     bool b1, b2, b3;
//     bif >> b1 >> b2 >> b3;
//
bistream& operator>>(bistream& bif, bool& b);

/*
 * OUTPUT
 */

// A bit output stream, for writing individual bits to a file.
class bostream {
public:
    // Writes a bit to this bit output stream.
    //
    // Parameters:
    //
    //      b - the bit to write
    //
    // Returns: the same bit output stream, for method chaining
    //
    // Example:
    //
    //      bof.write(true);
    //
    virtual bostream& write(bool b) = 0;

    // Writes an `n`-bit big-endian representation of `value`, which
    // must have a numeric type, to this bit output sream.
    //
    // Parameters:
    //
    //      <typename T> - the type of the value to write; must be numeric
    //      value - the value to write
    //      n - number of bits to write
    //
    // Returns: the same bit output stream, for method chaining
    //
    // Example:
    //
    //      bof.write(22, 5); // writes 10110
    //      bof.write(22, 6); // writes 010110
    //
    template<typename T>
    bostream& write_bits(T value, size_t n);

    // Determines the status of the bit output stream.
    //
    // Returns: `false` if an error has occurred; `true` otherwise
    //
    // Example:
    //
    //      bof.write(b);
    //      if (bof.good()) {
    //          // we know the write succeeded
    //      }
    //
    virtual bool good() const = 0;

    // The bit output stream boolean coercion operator, inserted, for
    // example, when a `bofstream` is used as a condition for an
    // `if`. Alias for `good()`.
    //
    // Returns: `false` if an error has occurred; `true` otherwise
    //
    // Example:
    //
    //      bof.write(b);
    //      if (bof) {
    //          // we know the write succeeded
    //      }
    //
    operator bool() const;

    virtual ~bostream() { }
};

// Adapts any `std::ostream&` for use as a bit output stream.
class bostream_adaptor : public bostream {
public:
    // Constructs a bit output stream to write to the given output stream.
    //
    // Parameters:
    //
    //      output_stream - the byte output stream to adapt
    //
    // Example:
    //
    //      bostream_adaptor bos(output_stream);
    //
    explicit bostream_adaptor(std::ostream&);

    virtual bostream_adaptor& write(bool b) override;

    virtual bool good() const override;

    ~bostream_adaptor();

    bostream_adaptor(bostream_adaptor const&) = delete;

private:
    uint8_t bitbuf_;
    size_t nbits_;
    std::ostream& stream_;

    void write_out_();
};

class bofstream : public bostream_adaptor {
public:
    // Constructs a bit output stream to write to the given file.
    //
    // Parameters:
    //
    //      filespec - name of the file to open or create
    //
    // Example:
    //
    //      bofstream bof(output_file_name);
    //
    explicit bofstream(char const* filespec);

    bofstream(bofstream const&) = delete;

private:
    std::ofstream base_;
};

class bostringstream : public bostream {
public:
    const std::vector<uint8_t>& data() const;

    virtual bostringstream& write(bool b) override;

    virtual bool good() const override;

    size_t bits_written() const;

private:
    size_t bits_written_ = 0;
    std::vector<uint8_t> data_;

};

// The bit output stream insertion operator; alias for
// `write(bool)`.
//
// Params:
//
//     - bof - the bit input stream to write to
//     - b - the bit to write
//
// Returns: the same bit output stream, for method chaining
//
// Example:
//
//     bof << true << false << false;
//
bofstream& operator<<(bofstream& bof, bool b);

/*
 * TEMPLATE IMPLEMENTATIONS
 */

template<typename T>
bistream& bistream::read_bits(T& result, size_t n)
{
    bool bit;

    result = 0;

    while (n--) {
        read(bit);
        result = (result << 1) | bit;
    }

    return *this;
}

inline bistream& operator>>(bistream& bif, bool& bit)
{
    return bif.read(bit);
}

template<typename T>
bostream& bostream::write_bits(T value, size_t n)
{
    while (n--) {
        write(value >> n & 1);
    }

    return *this;
}

inline bostream& operator<<(bostream& bof, bool bit)
{
    return bof.write(bit);
}

} // end namespace ipd
