#include "common.h"
#include "bit_io.h"
#include <map>
#include <queue>
#include <iostream>
#include <sstream>

#define DEFAULT_INFILE  "test-files/hamlet-ascii.txt.huff"
#define DEFAULT_OUTFILE "test-files/hamlet-ascii.txt.out"
using namespace ipd;
std::map<char,std::string> bitcode;
std::map<char,size_t> freq;
struct huffmannode{
    char c;
    size_t freq;
    huffmannode *left,*right;

    huffmannode(char c, size_t freq){
        left=right=NULL;
        this->c =c;
        this->freq = freq;
    }
};
struct compare{
    bool operator()(huffmannode* left, huffmannode* right)
    {
        return (left->freq > right->freq);
    }
};
std::priority_queue<huffmannode*, std::vector<huffmannode*>, compare> q;
void storecodes(struct huffmannode* root, std::string str){
    if(root==NULL){
        return;
    }
    if(root->c != '#')
        bitcode[root->c]=str;
    storecodes(root->left, str+"0");
    storecodes(root->right, str+"1");
}

void createHuffmantree(){
    struct huffmannode *left, *right, *top;
    for(auto v = freq.begin(); v!=freq.end(); v++){
        q.push(new huffmannode(v->first, v->second));
    }
    while(q.size()!=1){
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

void getFreq(bifstream& in){
    char c;
    std::string str;
    while (in.read_bits(c, 8)) {
        if(c =='#') break;
        str += c;
    }

    for(int i =0; i<str.size();i++){

        if(i>0 && str.at(i) == ':' && str.at(i+1) != ':'){
            char ch = str.at(i-1);
            std::string tmp ;
            while(str.at(i+1)!= ';'){
                tmp +=str.at(i+1);
                i++;
            }
            std::stringstream tmps(tmp);
            size_t fre = 0;
            tmps >> fre;
            freq[ch] = fre;
        }
    }

}

void decode(struct huffmannode* root,bifstream& in, std::ostream& out){
    char c;
    struct huffmannode* curr = root;
    while (in.read_bits(c, 8)) {

        if(c == '0'){
            curr = curr->left;
        }else{
            curr = curr->right;
        }
        if(curr->left==NULL && curr->right == NULL){
            out<< curr->c;
            curr = root;
        }
    }

}
int main(int argc, const char* argv[])
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
    decode(q.top(),in, out);
    return 0;
}
