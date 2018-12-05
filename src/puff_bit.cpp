#include "common.h"
#include "bit_io.h"
#include <map>
#include <queue>
#include <iostream>
#include <sstream>

#define DEFAULT_INFILE "test-files/hamlet-ascii.txt.t.huff"
#define DEFAULT_OUTFILE "test-files/hamlet-ascii.txt.out"
struct huffmannode;
using node_ptr = std::shared_ptr<huffmannode>;
using struct_node_ptr = std::shared_ptr<struct huffmannode>;

using namespace ipd;
std::map<char, std::string> bitcode;
std::map<char, size_t> freq;
struct huffmannode
{
  char c;
  size_t freq;
  node_ptr left, right;

  huffmannode(char c, size_t freq)
  {
    left = right = nullptr;
    this->c = c;
    this->freq = freq;
  }
};
struct compare
{
  bool operator()(node_ptr left, node_ptr right)
  {
    return (left->freq > right->freq);
  }
};
std::priority_queue<node_ptr, std::vector<node_ptr>, compare> q;
void storecodes(struct_node_ptr root, std::string str)
{
  if (root == nullptr)
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
  struct_node_ptr left, right, top;
  for (auto v = freq.begin(); v != freq.end(); v++)
  {
    q.push(std::make_shared<huffmannode>(v->first, v->second));
  }
  while (q.size() != 1)
  {
    left = q.top();
    q.pop();
    right = q.top();
    q.pop();
    top = std::make_shared<huffmannode>('#', left->freq + right->freq);
    top->left = left;
    top->right = right;
    q.push(top);
  }
  storecodes(q.top(), "");
}

void getFreq(bifstream &in)
{
  char c;
  std::string str;
  while (in.read_bits(c, 8))
  {
    if (c == '#')
      break;
    str += c;
  }

  for (int i = 0; i < str.size(); i++)
  {

    if (i > 0 && str.at(i) == ':' && str.at(i + 1) != ':')
    {
      char ch = str.at(i - 1);
      std::string tmp;
      while (str.at(i + 1) != ';')
      {
        tmp += str.at(i + 1);
        i++;
      }
      std::stringstream tmps(tmp);
      size_t fre = 0;
      tmps >> fre;
      freq[ch] = fre;
    }
  }
}

void decode(struct_node_ptr root, bifstream &in, std::ostream &out)
{
  bool c;
  struct_node_ptr curr = root;

  while (in.read_bits(c, 1))
  {
    if (!c)
    {
      curr = curr->left;
    }
    else
    {
      curr = curr->right;
    }
    if (curr->left == nullptr && curr->right == nullptr)
    {
      out << curr->c;
      curr = root;
    }
  }
}
int main(int argc, const char *argv[])
{
  const char *infile, *outfile;

  get_file_names(argc, argv, infile, outfile,
                 DEFAULT_INFILE, DEFAULT_OUTFILE);

  bifstream in(infile);
  assert_good(in, argv);

  std::ofstream out(outfile);
  assert_good(out, argv);

  getFreq(in);

  createHuffmantree();
  decode(q.top(), in, out);
  return 0;
}
