#include <bits/stdc++.h>
#include <sys/stat.h>
using namespace std;

class Huffman;

uint32_t cnt=0;

class node{
    unsigned char data;
    uint32_t freq;
    string code;
    node * left;
    node * right;
    uint32_t time=0;
    public:
    node(){ left=NULL,right=NULL; }
    friend class Huffman;
};

class Huffman{
    //root of huffman tree;
    node * root;

    //comparator class made private to be used only in huffman tree
    class comp{
        public:
        bool operator()(node * a,node * b){
            // return ( a->freq==b->freq ? a->data > b->data : a->freq > b->freq  ) ;
            return (a->freq==b->freq ? a->time > b->time : a->freq > b->freq) ;
        }
    };

    string inpfname,outfname;

    priority_queue<node*,vector<node*>,comp> pq;


    vector<node*> arr;

    //contains the content of input file;
    vector<uint16_t> inputdata;
    vector<unsigned char> saveContents;
    public:

    Huffman(string a, string b) : inpfname(a),outfname(b) { root=NULL; }

    void compress(){
            readinpfile();
            Huffmantree();
            encode();
            writeinoutput();
        return;
    }

    /*
        for compression all we need to do is
        read the contents of the input file and store the frequencies
        and create the huffman tree and finding encodings of characters from huffman tree
        and writing the encodings in the outfile by reafing contents of inpfile
        
        solving compression using 4 stages
        1)read input file and create frquency table
        2)push the contents into priority queue and create the huffman tree
        3) travarsing the tree to find the encodings
        4)writing back meta data and the encodings in output file;
    */

    void readinpfile(){
        //since each byte is 8 bits atmax 256 charcters can be used
        arr.resize(256,NULL);
        for(int32_t i=0;i<256;i++){
            arr[i]=new node();
            arr[i]->freq=0;
            arr[i]->data=i;
            arr[i]->time=cnt++;
        }

        ifstream file(inpfname,ios::binary);

        if(!file.is_open()){
            cerr<<"Error while opening the file"<<'\n';
            exit(1);
        }
        // cout<<1<<endl;
        unsigned char ch;
        while(file.read(reinterpret_cast<char*>(&ch),1)){
            inputdata.emplace_back(int(ch));
            arr[ch]->freq++;
        }
        // for(auto & c: inputdata){
        //     cout<<int(c)<<' ';
        // }cout<<endl;
        file.close();
        return;
    }

    void Huffmantree(){
        // non zero frequencies are pushed into priority queue
        for(auto & c: arr ){
            if(c->freq>0)pq.push(c);
        }
        
        // creating the huffmann tree
        while(pq.size()>1){
            node * l=pq.top();
            pq.pop();
            node * r=pq.top();
            pq.pop();
            root=new node();
            root->freq=l->freq + r->freq ;
            root->left=l,root->right=r;
            root->time=cnt++;
            pq.push(root);
        }
        //   cout<<root->freq<<endl;
        return;
    }

    // traversing the tree and encoding the non zero frequencies characters are done here;
    void encode(){
        if(root==NULL){
            cerr<<"EMPTY file case"<<'\n';
            exit(1);
        }
        dfs(root,"");
        // for(int i=0;i<256;i++)if(arr[i]->freq>0)cout<<arr[i]->code<<' '<<i<<endl;

        // cout<<root->left->left->data<<'\n';
        // cout<<arr[97]->code<<'\n';
        // for(auto & c: arr)if(c->freq>0)cout<<c->code<<' '<<int(c->data)<<endl;
        return;
    }

    void dfs(node * cur,string s){
        //i.e the node contains the character encode it .
        if(cur->left==NULL && cur->right==NULL ){
            cur->code=s;
            return;
        }
        if(cur->left!=NULL)dfs(cur->left,s+'0');
        if(cur->right!=NULL)dfs(cur->right,s+'1');
        return;
    }


    void writeinoutput(){
        

        ofstream file(outfname,std::ios::binary);

        if(!file.is_open()){
            cerr<<"Error while opening the output file"<<endl;
            exit(1);
        }


        string s="";

        /*
            format for the zipped file is 
            1)meta data   
            2)actual compressed data
            //
            meta data format 
            [total number of distinct charcters were present in input file]
            [character] [no : of bytes used for frquency][frquency] // 1byte + 1byte + varaible that lies btw (1 to 8 bytes)
            actual compressed data is to encode the data into new encoding ;


            the data stored in string is in binary which should be converted to bytes before storing in comprressed file. 
        */
        uint32_t noOFCharsUsed=0;
        map<uint16_t,pair<uint16_t,uint32_t>> mp;
           
        for(uint16_t i=0;i<256;i++){
            uint32_t temp=arr[i]->freq;
            if(temp==0)continue;

            noOFCharsUsed+=1;

            int16_t j=31;
            while(temp>>j==0)j--;
            // cout<<j<<endl;
            mp[i]={(j+8)/8 ,temp};
        }
        // cout<<mp.size()<<endl;
        //   cout<<noOFCharsUsed<<endl;
        // noOFCharsUsed=40;
         s+=   char(noOFCharsUsed);
        //  cout<<int(s.back())<<endl;
        for(auto & [k,v] : mp){
            s+=char(k);
            // cout<<int(s.back())<<' ';
            s+=char(v.first);
            // cout<<int(s.back())<<' ';

            if(v.first==1){
                s+= char(v.second);
            }else if(v.first==2){
               uint32_t freq16 = static_cast<uint16_t>(v.second);
            // Ensure little-endian byte order (adjust if needed)
            s += static_cast<char>(freq16 & 0xFF);
            s += static_cast<char>((freq16 >> 8) & 0xFF);

            }else{
                uint32_t freq=v.second;
                s += static_cast<char>(freq & 0xFF);
                s += static_cast<char>((freq >> 8) & 0xFF);
                s += static_cast<char>((freq >> 16) & 0xFF);
                s += static_cast<char>((freq >> 24) & 0xFF);
            }
                //  cout<<uint32_t(k)<<' '<<v.first<<' '<<v.second<<'\n';
            while(!s.empty()){
                char t=s[0];
                s.erase(0,1);
                file.put(t);
            }
        }
        //    cout<<s<<'\n';
        //   s+=(char)noOFCharsUsed;
        //   cout<<mp['a'].first<<'\n';
        string temp="";
        //  int cnt1=0;
        //  for(int i=0;i<256;i++)if(arr[i]->freq>0)cout<<arr[i]->code<<' '<<i<<endl;
        // cout<<inputdata.size()<<' '<<s.size()<<endl;
        //  return;
        for(auto & c: inputdata){
            // cout<<arr[c]->code<<' ';
            if(c<0 ||c>=256){
                cout<<-1<<endl;
                return;
            }
            temp+=arr[c]->code;
            while(temp.size()>8){
                string AppendToS=temp.substr(0,8);
                //   cout<<AppendToS;
                file.put(convertTOChar(AppendToS));
                temp.erase(0,8);
                //  cnt1++;
            }
        } 
        //converting to 8 divisible 
        uint32_t appi=0;
        // cout<<appi<<endl;
        // cout<<10<<endl;
        if((!temp.empty())&&temp.size()%8!=0){
            // cout<<temp;
            appi=8-(temp.size()%8);
            temp+=string(8-(temp.size()%8),'0');
             
            
        }
        // cout<<endl<<appi<<endl;
         

        if(!temp.empty()){
            //  cout<<temp;
            file.put(convertTOChar(temp));
            file.put(char(appi));
            //  cnt1++;
        } 
        //   cout<<appi<<endl;
        //   cout<<int(s.back())<<endl;
        // converttodecimal(s);
        // cout<<'\n';
        //    cout<<s<<' ';
            // cout<<s.size()<<' '<<int(s[0])<<endl;
            // cout<<appi<<' '<<cnt1<<endl;


       
         
         
        // cout<<26<<endl;
        file.close();

        // struct stat file_stat;
        // if (stat(outfname.c_str(), &file_stat) == 0) {
        //     std::cout << "File size: " << file_stat.st_size << " bytes" << std::endl;
        // } 

        return;
    }

    char convertTOChar(string  str){
        uint32_t ans=0;
        //converting meta data from binary to bytes of characters
        for(int i=7;i>=0;i--){
            ans=ans *2 ;
            ans+=(str[i]=='1');
        }
        return char(ans);
    }


    /*
       for decompression first we need to restore the frequencies and create the tree 
       and trace the characters of the original file by decoding and writing back in a new file.
    */

    void decompress(){

        int j=0;
        restoreContents(j);
        createTree();
        decodingAndWrite(j);
        //  writeBack();
          
        return;
    }

    void restoreContents(int & temp){

        arr.resize(256);
        for(int32_t i=0;i<256;i++){
            arr[i]=new node();
            arr[i]->freq=0;
            arr[i]->data=i;
            arr[i]->time=cnt++;
        }

        ifstream file(inpfname,ios::binary);

        // std::streampos size = file.tellg();
        //     cout<<size<<endl;

        if(!file.is_open()){
            cerr<<"Unable to open file"<<'\n';
            exit(1);
        }
        unsigned char ch;
        file.read(reinterpret_cast<char * >(&ch),1);
         temp++;
        uint16_t noOFchars=int(ch);
        //   cout<<noOFchars<<'\n';
        for(int i=0;i<noOFchars;i++){
             
            file.read(reinterpret_cast<char * >(&ch),1);
            int32_t Char=int(ch);
            file.read(reinterpret_cast<char * >(&ch),1);
            uint16_t j=int(ch);
            temp+=2;
            uint32_t fre;

            if (j > 2) {
                char32_t wideChar;
                file.read(reinterpret_cast<char*>(&wideChar), sizeof(char32_t));
                temp+=4;
                fre = static_cast<uint32_t>(wideChar);
            } else if (j == 2) {
                char16_t wideChar;
                file.read(reinterpret_cast<char*>(&wideChar), sizeof(char16_t));
                temp+=2;
                fre = static_cast<uint16_t>(wideChar);
            } else {
                file.read(reinterpret_cast<char*>(&ch), sizeof(char));
                temp++;
                fre = static_cast<uint8_t>(ch);
            }

             
            arr[Char]->freq=fre;
            //   cout<<fre<<' '<<Char<<' '<<j<<endl;
        }

        unsigned char cha;
        while(file.read(reinterpret_cast<char*> (&cha),1)){
            saveContents.push_back(cha);
        }

        //  cout<<int(saveContents.back())<<endl;
        // cout<<saveContents.size()<<endl;

        file.close();
        return;
    }

    void createTree(){
        for(int i=0;i<256;i++){
            if(arr[i]->freq>0)pq.push(arr[i]);
        }
        // cout<<pq.size()+1<<' '<<pq.top()->freq<<endl;
        while(pq.size()>1){
            auto l=pq.top();
            pq.pop();
            auto r=pq.top();
            pq.pop();
            root=new node();
            root->freq=l->freq+r->freq;
            root->left=l,root->right=r;
            root->time=cnt++;
            pq.push(root);
        }
        // encode();
        // cout<<20<<endl;
        //   cout<<pq.top()->freq<<'\n';
        return;
    }

    void decodingAndWrite(int pos) {
         

        ofstream file1(outfname, ios::binary| ios::out);
        if (!file1) {
            cerr << "Failed to open output file!" << endl;
            return;
        }
        
        int temp=0,idx=0;
        string append=std::bitset<8>(int(saveContents.back())).to_string();
         
         while(idx<8){
            temp*=2;
            temp+=(append[idx++]=='1');
         }
        //  cout<<temp<<endl;
        node* travel = root;
        
        for(int i=0;i<saveContents.size()-1;i++){
            string path=std::bitset<8>(int(saveContents[i])).to_string();
             reverse(path.begin(),path.end());
           
            if(i==saveContents.size()-2)path=path.substr(0,8-(temp));
            //  cout<<path;
            for(int j=0;j<path.size();j++){
                travel=(path[j]=='0' ? travel->left : travel->right);
                if(travel->left==NULL&&travel->right==NULL){
                    file1.put(travel->data);
                    travel=root;
                }
            }
        }
        
 
         

         
        file1.close();
        return;
    }


};