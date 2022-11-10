#include "simulator.h"

Simulator::Simulator(Cache l1, Cache l2)
: l1_cache(l1), l2_cache(l2){
    // l2_size = l2_size;
}

string Simulator::bin2hex(string& binary){
    ostringstream a;
    int dec = stoi(binary, 0, 2);
    a<< hex << dec << endl;
    string hex_ = a.str();
    return hex_;
}

vector<string> Simulator::split(string& s, string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void Simulator::beginSimulator(string& trace_file, unsigned long int& inclusion){
    // cout << l1_cache.size << " siml " << l2_cache.size << endl;
    ifstream scanner;
    string trace;
    scanner.open(trace_file);
    if (scanner.is_open()){
        while (getline(scanner, trace)){
            if(trace == "") continue;
            counter++;
            string delimeter = " ";
            vector<string> trace_content = split(trace, delimeter);
            string operation = trace_content[0];
            string address = trace_content[1];
            bool l1_status = false;
            bool l2_status;
            string out;
            string ad;
            // if (counter == 50){
            //     break;
            // }
            if (operation == "w") {
                l1_status = l1_cache.writeContent(address);
            }
            else if (operation == "r") {
                l1_status = l1_cache.readContent(address);
            }
            if (l1_status == false && l2_cache.size == 0) l1_cache.allocateContent(address, operation, counter);
            else if (l1_status == false && l2_cache.size != 0){
                out = l1_cache.allocateContent(address, operation, counter);
                if (out != ""){
                    ad = bin2hex(out);
                    l2_status = l2_cache.writeContent(ad);
                    if(l2_status == false){
                        operation = "w";
                        l2_cache.allocateContent(ad, operation, counter);
                    }
                }
                l2_status = l2_cache.readContent(address);
                if (l2_status==false){
                    out = l2_cache.allocateContent(address, operation, counter);
                    if (out != "" && inclusion == 1){
                        ad = bin2hex(out);
                        l1_cache.makeInvalid(ad);
                    }
                }
            }
        }
    }
}

void Simulator::printResults(){
    cout << "------------ L1 Contents ------------" << endl;
    printCacheContents(l1_cache);
    if (l2_cache.size != 0){
        cout << "------------ L2 Contents ------------" << endl;
        printCacheContents(l2_cache);
    }
    cout << "------------ Simulation Results (raw) ------------" << endl;
    cout << "a. number of L1 reads              : " << l1_cache.read << endl;
    cout << "b. number of L1 read misses        : " << l1_cache.read_miss << endl;
    cout << "c. number of L1 writes             : " << l1_cache.write << endl;
    cout << "d. number of L1 writes misses      : " << l1_cache.write_miss << endl;
    double l1_miss_rate = (double) (l1_cache.read_miss + l1_cache.write_miss) / (double) (l1_cache.read + l1_cache.write);
    cout << "e. L1 miss rate                    : " << l1_miss_rate << endl;
    cout << "f. number of L1 writebacks         : " << l1_cache.write_back <<endl;
    unsigned long int traffic = l1_cache.read_miss + l1_cache.write_miss + l1_cache.write_back;
    if(l2_cache.size != 0){
        cout << "g. number of L2 reads              : " << l2_cache.read << endl;
        cout << "h. number of L2 read misses        : " << l2_cache.read_miss << endl;
        cout << "i. number of L2 writes             : " << l2_cache.write << endl;
        cout << "j. number of L2 writes misses      : " << l2_cache.write_miss << endl;
        double l2_miss_rate = (double) (l2_cache.read_miss) / (double) (l2_cache.read);
        cout << "k. L2 miss rate                    : " << l2_miss_rate << endl;
        cout << "f. number of L2 writebakcs         : " << l2_cache.write_back <<endl;
        traffic = l2_cache.read_miss + l2_cache.write_miss + l2_cache.write_back + l1_cache.write_back_m;
    }
    else{
        cout << "g. number of L2 reads              : " << 0 << endl;
        cout << "h. number of L2 read misses        : " << 0 << endl;
        cout << "i. number of L2 writes             : " << 0 << endl;
        cout << "j. number of L2 writes misses      : " << 0 << endl;
        cout << "k. L2 miss rate                    : " << 0 << endl;
        cout << "l. number of L2 writebakcs         : " << 0 <<endl;
    }
    cout << "m. total memory traffic             : " << traffic << endl;

}

void Simulator::printCacheContents(Cache c){
    for(unsigned long int i=0;i<c.sets;i++){
        cout << "Set\t" << i << " : " ;
        for(unsigned long int j=0;j<c.asc;j++){
            if (c.cache[i][j].valid == true){
                string hx = bin2hex(c.cache[i][j].address_tag);
                cout << hx << " ";
                if (c.cache[i][j].dirty == 1){
                    cout << " D\t";
                }
                else{
                    cout << "\t";
                }
            }
        }
        cout << endl;
    }
}
