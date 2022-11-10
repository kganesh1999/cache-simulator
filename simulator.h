#ifndef SIMULATOR_H
#define SIMULATOR_H
#include <fstream>
#include <sstream>
#include "cache.h"
using namespace std;
class Simulator{
    public:
        Cache l1_cache;
        Cache l2_cache;
        unsigned long int l2_size;
        unsigned long int counter = 0;
        Simulator(Cache l1, Cache l2);
        string bin2hex(string& binary);
        vector<string> split(string& s, string& delimeter);
        void beginSimulator(string& trace_file, unsigned long int& inclusion);
        void printResults();
        void printCacheContents(Cache c);
};
#endif