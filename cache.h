#ifndef CACHE_H
#define CACHE_H
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <math.h> 
#include <vector>
using namespace std;

class Block{
    public:
        string address_tag;
        string tag_index;
        bool valid;
        bool dirty;
        Block(){
            address_tag = "";
            tag_index = "";
            valid = false;
            dirty = false;
        }
};

class Cache{
    public:
        unsigned long int sets, asc, tag, index, offset, size;
        string tag_index, trace_file;
        vector<string> tags;
        unsigned long int read = 0;
        unsigned long int read_miss= 0;
        unsigned long int write = 0;
        unsigned long int write_miss = 0;
        unsigned long int write_back = 0;
        unsigned long int write_back_m = 0;
        unsigned long int rp;
        unsigned long int count = 0; 
        unsigned long int** lru;
        Block** cache;
        Cache(unsigned long int block_size, unsigned long int cache_size, unsigned long int assoc, unsigned long int replacement_policy, string trace);
        vector<string> split(string& s, string& delimiter);
        vector<string> addressParser(string& address);
        int getLRU(unsigned long int& set);
        int getPLRU(unsigned long int& set);
        int getOptimal(unsigned long int& idx, unsigned long int& counter);
        void updateLRU(unsigned long int& set, unsigned long int& assoc);
        void updatePLRU(unsigned long int& set, unsigned long int& idx);
        void makeInvalid(string& address);
        bool readContent(string& address);
        bool writeContent(string& address);
        string allocateContent(string& address, string& op, unsigned long int& counter);
        string allocateReplacement(string& address, string& op, unsigned long int& counter);
};

#endif
