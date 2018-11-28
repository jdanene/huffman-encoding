#pragma once

#include <cstdlib>
#include <cstdio>

template <class Stream>
void assert_good(const Stream& stream, const char* argv[])
{
    if (! stream.good()) {
        perror(argv[0]);
        exit(1);
    }
}

inline void get_file_names(int argc, const char* argv[],
                           const char*& infile, const char*& outfile,
                           const char* default_in, const char* default_out)
{
    switch (argc) {
    // If there are two command-line arguments then they are the files
    // to read from and write to:
    case 3:
        infile = argv[1];
        outfile = argv[2];
        return;

    // If there are no command-line arguments then we use the default
    // input and output files:
    case 1:
        infile = default_in;
        outfile = default_out;
        return;

    // Any other number of arguments is a user error, so we print a help
    // message:
    default:
        fprintf(stderr, "Usage: %s [ INFILE OUTFILE ]\n", argv[0]);
        exit(1);
    }
}
