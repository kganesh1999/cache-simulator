#include <iostream>
#include <string>
#include "simulator.h"
#include "cache.h"
using namespace std;

int main(int argc, char* argv[]){
    try{
        if (argc-1==8){
            unsigned long int block_size = atoi(argv[1]);
            unsigned long int l1_size = atoi(argv[2]);
            unsigned long int l1_assoc = atoi(argv[3]);
            unsigned long int l2_size = atoi(argv[4]);
            unsigned long int l2_assoc = atoi(argv[5]);
            unsigned long int replacement_policy = atoi(argv[6]);
            unsigned long int inclusive_property = atoi(argv[7]);
            string trace_file = argv[8];
            cout << "------------ Simulator Configurations ------------" << endl;
            cout << "BLOCK SIZE          : " << block_size << endl;
	        cout << "L1 SIZE             : " << l1_size << endl;
	        cout << "L1 ASSOC            : " << l1_assoc << endl;
            cout << "L2 SIZE             : " << l2_size << endl;
	        cout << "L2 ASSOC            : " << l2_assoc << endl;
            if (replacement_policy==0) cout << "REPLACEMENT POLICY  : " << "LRU" << endl;
            else if (replacement_policy==1) cout << "REPLACEMENT POLICY   : " << "Pseudo-LRU" << endl;
            else cout << "REPLACEMENT POLICY  : " << "Optimal" << endl;
            if (inclusive_property==0) cout << "INCLUSION PROPERTY  : " << "Non-inclusive" << endl;
            else cout << "INCLUSION PROPERTY  : " << "Inclusive" << endl;
            cout << "Trace file          : " << trace_file << endl;
            Cache* l1 = new Cache(block_size, l1_size, l1_assoc, replacement_policy, trace_file);
            Cache* l2 = new Cache(0, 0, 0, replacement_policy, trace_file);

            if (l2_size != 0){
                delete l2;
                Cache* l2 = new Cache(block_size, l2_size, l2_assoc, replacement_policy, trace_file);
            }
            Simulator *simulator = new Simulator(*l1, *l2);
            simulator->beginSimulator(trace_file, inclusive_property);
            simulator->printResults();
        }else{
            throw(argc-1);
        }
    }catch (int args_count) {
        cout << "Should contain exactly 8 arguments!!" << " But " << args_count << " are entered." <<endl;
    }
    return 0;
}
