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
std::map<char, int> freq;
struct huffmannode
{
    char c;
    int freq;
    huffmannode *left, *right;

    huffmannode(char c, int freq)
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
void printCodes(struct huffmannode *root, std::string str)
{
    if (!root)
        return;
    if (root->c != '#')
        std::cout << root->c << ": " << str;
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
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
    printCodes(q.top(), "");
}
void buildfreqtable(std::istream &in)
{
    char c;
    while (in.read(&c, 1))
    {
        freq[c]++;
    }
}
void writefreqtabletofile(bostream &out)
{
    for (auto v = freq.begin(); v != freq.end(); v++)
    {
        //write to out file;
    }
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
    createHuffmantree();

    return 0;
}
