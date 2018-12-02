#include "common.h"
#include "bit_io.h"
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>

#define DEFAULT_INFILE "test-files/hamlet-ascii.txt.huff"
#define DEFAULT_OUTFILE "test-files/hamlet-ascii.txt.puff"

using namespace ipd;
std::map<char, std::string> bitcode;
std::map<char, size_t> freq;
struct huffmannode
{
    char c;
    size_t freq;
    huffmannode *left, *right;

    huffmannode(char c, size_t freq)
    {
        left = right = NULL;
        this->c = c;
        this->freq = freq;
    }
};
struct compare
{
    bool operator()(huffmannode *left, huffmannode *right)
    {
        return (left->freq > right->freq);
    }
};

void storecodes(struct huffmannode *root, std::string str)
{
    if (root == NULL)
    {
        return;
    }
    if (root->c != '#')
        bitcode[root->c] = str;
    storecodes(root->left, str + "0");
    storecodes(root->right, str + "1");
}
void createHuffmantree()
{
    struct huffmannode *left, *right, *top;
    std::priority_queue<huffmannode *, std::vector<huffmannode *>, compare> q;

    for (auto v = freq.begin(); v != freq.end(); v++)
    {
        q.push(new huffmannode(v->first, v->second));
    }
    while (q.size() != 1)
    {
        left = q.top();
        q.pop();
        right = q.top();
        q.pop();
        top = new huffmannode('#', left->freq + right->freq);
        top->left = left;
        top->right = right;
        q.push(top);
    }
    storecodes(q.top(), "");
}
void buildfreqtable(std::istream &in)
{
    char c;
    while (in.read(&c, 1))
    {
        freq[c]++;
    }
}
void writecodes(std::istream &in, bostream &out)
{
    char c;
    in.clear();
    in.seekg(0, std::ios::beg);
    while (in.read(&c, 1))
    {
        std::string code = bitcode[c];
        for (int i = 0; i < code.size(); i++)
        {
            out.write_bits(code.at(i), 8);
        }
    }
}
void writefreqtabletofile(bostream &out)
{
    for (auto v = freq.begin(); v != freq.end(); v++)
    {
        char ch = v->first;
        std::string f = std::to_string(v->second);

        out.write_bits(ch, 8);
        out.write_bits(':', 8);
        for (int i = 0; i < f.size(); i++)
        {
            out.write_bits(f.at(i), 8);
        }
        out.write_bits(';', 8);
    }
    out.write_bits('#', 8);
}
int main(int argc, const char *argv[])
{
    const char *infile, *outfile;

    get_file_names(argc, argv, infile, outfile,
                   DEFAULT_INFILE, DEFAULT_OUTFILE);

    std::ifstream in(infile);
    assert_good(in, argv);

    bofstream out(outfile);
    assert_good(out, argv);

    buildfreqtable(in);
    writefreqtabletofile(out);

    createHuffmantree();
    writecodes(in, out);

    return 0;
}
