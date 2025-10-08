#include "huffman.cpp"
using namespace std;



int32_t main(int argc,char * argv[]){
    if(argc!=3){
        cerr<<"Failed to detect files"<<'\n';
        exit(1);
    }

    Huffman h1(argv[1],argv[2]);

    h1.decompress();


    return 0;
}