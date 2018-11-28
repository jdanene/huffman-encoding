# IPD Homework 8: Huffman Coding

  - **Due: Tuesday, December 4 at 11:59 PM**

## Summary

For this homework, you will write two programs:

  - `huff` compresses files using Huffman coding.

  - `puff` decompresses Huffman-coded files produced by `huff`, giving
    back the original file.

The filenames to read from and write to are passed to each program as
command-line arguments. For example, suppose you have a text file
`hamlet.txt` that you'd like to compress. You can compress it to a file
`hamlet.txt.huff` using your `huff` program:

```sh
    % ./huff hamlet-utf8.txt hamlet-utf8.txt.huff
```

You can decompress it using your `puff` program:

```sh
    % ./puff hamlet-utf8.txt.huff hamlet-utf8.txt.out
```

If you’ve done your job correctly, the decompressed file will match the
original. On UNIX (including Linux and Mac OS X) you can compare the two
to make sure they match using `diff`:

```sh
    % diff hamlet-utf8.txt hamlet-utf8.txt.out
```

When two files match, `diff` prints nothing.

### Goals

The primary goal of this assignment is to get you programming with
pointers. You will also become familiar with a specific data structure,
the Huffman tree, and you will think about how data structures can be
serialized—that is, converted to a sequence of bits that can be written
to a file.

### Course policy review

Please review [the collaboration
policies](http://users.eecs.northwestern.edu/~jesse/course/ipd/#policies)
for IPD before starting. In particular, note that you should not use
code from outside sources, and if you look at code from outside
sources then you must cite it.

## Getting started

This homework assignment does not include reference material for Huffman
trees and codes. Instead, you should start with the
[lecture notes](../../lectures/lec17/README.md) or the
[Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding#Basic_technique)
page on Wikipedia.

This repository contains starter code, including bitwise IO libraries
and sample encoding and decoding programs. Note that the bitwise IO
libraries come with support to help you write tests for end-to-end
encoding and decoding. Take advantage of it.

The starter code includes a pair of programs `encode` and `decode`
that---rather than Huffman coding---compress an ASCII text file to 7/8
of its original size using a simple [block
code](https://en.wikipedia.org/wiki/Block_code). (Note that your
`huff` and `puff` programs are not limited to ASCII files, but must
work on binary files containing any sequence of bytes.) The next
section discusses details of how that works.

## Warmup: ASCII block code

Most computers today have 8-bit bytes, but the
[ASCII](https://en.wikipedia.org/wiki/ASCII) text encoding requires only
7 bits per character. ASCII text files use one 8-bit byte for each 7-bit
character, leaving the high-order bit unused. (Eight-bit text encodings
such as [Latin-1](https://en.wikipedia.org/wiki/ISO/IEC_8859-1) use the
high bit to double the number of representable characters for writing a
variety of Latin-alphabet–based languages other than English.) Thus,
each ASCII character is 7 bits of information represented in an 8-bit
block code.

Given ASCII text (rather than, say, Latin-1), we can avoid storing each
byte's high bit using a 7-bit code word, and pack them into bytes. For
example, the string “ABCDEFGH” is represented in ASCII using 8 bytes,
one for each character, with the values 65, 66, 67, 68, 69, 70, 71, and
72. We can write these bytes out in binary like so:

```
01000001 01000010 01000011 01000100 01000101 01000110 01000111 01001000
```

Because ASCII does not use the high (*i.e.,* leftmost) bit of each byte,
the first bit in each byte is 0. Since it will never be 1, we don't need
to represent it and can leave it out:

```
1000001 1000010 1000011 1000100 1000101 1000110 1000111 1001000
```

Bits in files are grouped into bytes, which means that to store this
sequence of 56 bits without wasting bits, we’ll have to store it not as
eight groups of seven but as seven groups of eight. That is, we pack the
bits into bytes:

```
10000011 00001010 00011100 01001000 10110001 10100011 11001000
```

The decimal values of these seven bytes are 131, 10, 28, 72, 177, 162,
and 200. Only 10, 28, and 72 (the three bytes whose high bit is 0)
represent characters in ASCII: the newline, a legacy [control
code](https://en.wikipedia.org/wiki/Control_character) called “FS,” and
the letter `H`. Were we to open a file with these seven bytes in a text
editor, it might interpret them in one of myriad [extensions of
ASCII](https://en.wikipedia.org/wiki/Extended_ASCII) that uses the high
bit to include additional characters. In the most common such extension,
[Latin 1](https://en.wikipedia.org/wiki/ISO/IEC_8859-1), those seven
bytes might appear in a text editor like so:

```
�
H¢±È
```

The program `encode` converts from the ordinary 8-bit format to the
packed 7-bit format, and `decode` converts back. Each of them can be passed 
two command-line arguments, and input filename and an output filename; if 
none is passed, they use the hard-coded defaults. Note that the programs 
expect to find test input files at the relative path `test-files/`, which 
means that if you are using CLion, you will have to edit the run 
configuration for each to set the working directory to be the project directory.

Additionally, there are two caveats:

  - This encoding means that if your file has any non-ASCII characters,
    they won’t survive the encoding.

  - For some file sizes, the round-tripped (encoded and then decoded)
    file has an additional, final byte that wasn't in the original file.

Consider the following questions, and feel free to discuss on Piazza:

  - What is the problem with `encode` and `decode`? When is the extra
    byte added, and why?

  - Can you think of a way to fix the problem? Assume that you are
    allowed to change the file format used by `encode` and `decode` so
    that the number of bits added to the encoded file is *bounded by a
    constant*. (That is, the number of bits added should not depend on
    the original file size.)

    Can you think of a fix that works even if the file is
    streaming? That is, suppose that you cannot see the whole file at
    once, but rather must begin producing output before the input ends.

To try this out, use the included hamlet text files. The file
hamlet-utf8.txt demonstrates the first problem and the file
hamlet-ascii.txt demonstrates the second problem.

## The Huffman coder and decoder

Your main task is to write the two programs `huff` and `puff` as
described in the introduction. In particular:

  - `huff` reads an unencoded input file, and writes the Huffman-coded
    results an output file.

  - `puff` reads a Huffman-coded input file, and writes the decoded
    results an output file.

When `huff` reads an input file, it reads it as a sequence of bytes,
that is, values ranging from 0 to 255. In coding terms, the input
language’s alphabet has 256 different symbols. `huff` counts how many
times each of these 256 symbols occurs in the file and uses these counts
as weights to construct a Huffman tree; thus, the leaves of the tree
will be byte values. It then uses the Huffman tree to write an encoded
version of the input file to its output file.

The job of `puff` is to read a `huff`-encoded file and produce the
original file. However, there are two reasons this task is harder than
it may sound:

1.  The number of bits that the encoding requires may not be a multiple
    of 8, in which case the remaining bits in the file will be padded
    with 0s. How can the decoder tell padding bits from data bits?

2.  The decoder needs some way to recover the same Huffman tree that
    `huff` used to encode it. Since different files have different
    letter frequencies, `huff` needs to somehow transmit the tree to
    `puff` in a file header—that is, metadata written at the beginning
    of a file before the main data starts. Designing the file format is
    up to you. Here are two ideas:

      - Serialize (that is, convert to a sequence of bits) the tree
        using a pre-order traversal, as follows:

          - At every leaf, output a 0 bit followed by the 8 bits
            representing the input symbol on that leaf.

          - At every branch, output a 1 bit, followed by left subtree
            and then the right subtree.

      - Instead of storing the tree, store the frequency table used to
        build the tree. Provided your Huffman-tree algorithm is
        deterministic, you should be able to reconstruct the same tree
        in `puff` as was used in `huff`.

Feel free to discuss these questions on Piazza. Note that because you
are designing your own file format, your file format will not
necessarily match that of other students.

## Deliverables

Your deliverables are:

  - The source code of the `huff` and `puff` programs as described
    above.

  - A plain text file `EVALUATION.md` (which can use
    [Markdown](https://help.github.com/articles/github-flavored-markdown/)
    formatting if you like) in which you describe the design of your
    program. In particular please discuss, briefly:

      - how you tested your programs to ensure correctness and

      - anything that surprised you while doing this assignment.

    When discussing your code, please provide file and line number
    references.

## How to submit

Please submit a ZIP or TAR archive of your whole project. Your project
should build in CLion using the configuration in CMakeLists.txt. Before
creating the archive, be sure to clean your project (*Run* menu /
*Clean*).
