#include "7coder.h"
#include "common.h"

#include <iostream>
#include <cstdio>

// These are the default files to read from and write to when no
// command-line arguments are given:
// change "ascii" to "utf8" to see how this goes wrong with 8 bit files
#define DEFAULT_INFILE  "test-files/hamlet-ascii.txt.huff"
#define DEFAULT_OUTFILE "test-files/hamlet-ascii.txt.puff"

using namespace ipd;
using namespace std;

int main(int argc, const char *argv[])
{
    const char *infile, *outfile;

    get_file_names(argc, argv, infile, outfile,
                   DEFAULT_INFILE, DEFAULT_OUTFILE);

    bifstream in(infile);
    assert_good(in, argv);

    ofstream out(outfile);
    assert_good(out, argv);

    decode(in, out);
}

