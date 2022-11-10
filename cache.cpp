#include "cache.h"

Cache::Cache(unsigned long int block_size, unsigned long int cache_size, unsigned long int assoc, unsigned long int replacement_policy, string trace){
    if (cache_size!=0){
        trace_file = trace;
        size = cache_size;
        sets = cache_size / (block_size * assoc);
        asc = assoc;
        rp = replacement_policy;
        cache = new Block* [sets];
        for (unsigned long int i=0; i<sets; i++){
            cache[i] = new Block[asc];
        }
        offset = (unsigned long int) log2(block_size);
        index = (unsigned long int) log2(sets);
        tag = 32-index-offset;
        if (rp == 0 || assoc == 1){
            lru = new unsigned long int* [sets];
            for (unsigned long int i=0; i<sets; i++){
                lru[i] = new unsigned long int[asc];
            }
        }else if (rp ==1){
            lru = new unsigned long int* [sets];
            for (unsigned long int i=0; i<sets; i++){
                lru[i] = new unsigned long int[asc-1];
            }
        }else if (rp ==2){
            ifstream scanner;
            string trace;
            scanner.open(trace_file);
            if (scanner.is_open()){
                while (getline(scanner, trace)){
                    if(trace == "") continue;
                    string delimeter = " ";
                    vector<string> trace_content = split(trace, delimeter);
                    string address = trace_content[1];
                    string tag = addressParser(address)[0];
                    tags.push_back(tag);
                }
            }
    }
}
}

vector<string> Cache::split(string& s, string& delimiter) {
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

vector<string> Cache::addressParser(string& address){
    vector<string> v;
    unsigned long int a_int = stoi(address, 0, 16);
    string binarystring = bitset<32>(a_int).to_string();
    string tg = binarystring.substr(0, tag);
    string idx = binarystring.substr(tag, index);
    v.push_back(tg);
    v.push_back(idx);
    return v;
}

int Cache::getLRU(unsigned long int& set){
    unsigned long int min_idx = 0;
    for(unsigned long int i=0;i < asc; i++){
        if (lru[set][i] < lru[set][min_idx]) min_idx = i;
    }
    return min_idx;
}

void Cache::updateLRU(unsigned long int& set, unsigned long int& idx){
    lru[set][idx] = count;
    count++;
    
}

int Cache::getPLRU(unsigned long int& set){
    string x = "";
    int i = 1;
    while(i-1 < asc-1){
        if (lru[set][i-1] == 0){
            x += "1";
            i = (2*i);
        }
        else{
            x+= "0";
            i = (2*i)+1;
        }
    }
    return stoi(x,0,2);
}

void Cache::updatePLRU(unsigned long int& set, unsigned long int& idx){
    int l = (int) ceil(log2(asc));
    string bits = bitset<32>(idx).to_string();
    bits = bits.substr(bits.size()-l);
    int j = 1;
    for(int i=0; i<l; i++){
        lru[set][j-1] = int(bits[i]);
        if (bits[i] == '0') j = 2*j;
        else j = (2*j)+1;       
    }
}

int Cache::getOptimal(unsigned long int& index, unsigned long int& counter){
    unsigned long int temp[asc];
    unsigned long int count = 0;
    for(unsigned long int j=0; j< asc; j++){
        for(unsigned long int i = counter-1; i<tags.size(); i++){
            if(cache[index][j].address_tag == tags[i]){
                temp[j] = i;
                count++;
                break;
            }
        }
        if(count==j) return j;
    }
    int max_idx = 0;
    for(unsigned long int k=0; k<asc; k++){
        if(temp[k] > temp[max_idx]) max_idx = k;
    } 
    return max_idx;
}

void Cache::makeInvalid(string& address){
    unsigned long int out = 0;
    unsigned long int idx = 0;
    vector<string> parsed = addressParser(address);
    string tg = parsed[0];
    string idxx = parsed[1];
    if(sets!=1){
        idx = stoi(idxx, 0, 2);
    }
    for(unsigned long int i=0;i<asc;i++){
        if(cache[idx][i].address_tag.compare(tg) == 0){
            cache[idx][i].valid = false;
            out = cache[idx][i].dirty;
            break;
        }
    }
    if (out == true) write_back_m++;
    
}

bool Cache::readContent(string& address){
    bool status = false;
    vector<string> parsed = addressParser(address);
    unsigned long int idx = 0;
    read++;
    string tg = parsed[0];
    string idxx = parsed[1];
    if (sets != 1){
        idx = stoi(idxx, 0, 2);
    }
    for(unsigned long int i = 0; i < asc; i++){ 
        if(cache[idx][i].valid == true){
            if(cache[idx][i].address_tag == tg){
                if (rp == 0 || asc == 1){
                    updateLRU(idx, i);
                    
                } else if (rp==1){
                    updatePLRU(idx, i);
                    
                }
                status = true;
                break;
            }
        }
    }
    if (status == false) read_miss++; 
    return status;
}

bool Cache::writeContent(string& address){
    bool status = false;
    vector<string> parsed = addressParser(address);
    unsigned long int idx = 0;
    write++;
    string tg = parsed[0];
    string idxx = parsed[1];
    if (sets != 1){
        idx = stoi(idxx, 0, 2);
    }
    for(unsigned long int i = 0; i < asc; i++){
        if(cache[idx][i].valid == true){
            if(cache[idx][i].address_tag == tg){
                if (rp == 0 || asc == 1){
                    updateLRU(idx, i);
                } else if (rp==1){
                    updateLRU(idx, i);
                }
                status = true;
                break;
            }
        }
    }
    if (status == false) write_miss++;
    return status;
}

string Cache::allocateContent(string& address, string& op, unsigned long int& counter){
    // cout << "alloccontent" << endl;
    bool allocation_status = false;
    unsigned long int idx = 0;
    vector<string> parsed = addressParser(address);
    string tg = parsed[0];
    string idxx = parsed[1];
    if (sets != 1) { idx = stoi(idxx, 0, 2);}
    for(unsigned long int i = 0; i < asc; i++){
        if(cache[idx][i].valid == false){
            cache[idx][i].address_tag = tg;
            cache[idx][i].tag_index = tg+idxx;
            cache[idx][i].valid = true;
            allocation_status = true;
            if (rp == 0 || asc == 1){
                    updateLRU(idx, i);
            } else if (rp==1){
                    updatePLRU(idx, i);
            }
            if (op == "w") cache[idx][i].dirty = true;
            break;
            }
        }
        if (allocation_status==false) return allocateReplacement(address, op, counter);
        
        return "";
}

string Cache::allocateReplacement(string& address, string& op, unsigned long int& counter){
    unsigned long int idx = 0;
    vector<string> parsed = addressParser(address);
    string tg = parsed[0];
    string idxx = parsed[1];
    if (sets != 1) idxx = stoi(idxx, 0, 2);
    string out = "";
    unsigned long int r_idx = 0;
    if (rp==0 || asc == 1) r_idx = getLRU(idx);
    else if (rp == 1) r_idx = getPLRU(idx);
    else if (rp == 2) r_idx = getOptimal(idx, counter);
    if (cache[idx][r_idx].dirty == true){
        write_back ++;
        out = cache[idx][r_idx].tag_index;
    }
    cache[idx][r_idx].address_tag = tg;
    cache[idx][r_idx].tag_index = tg+idxx;
    cache[idx][r_idx].valid = true;
    if (rp==0 || asc == 1) updateLRU(idx, r_idx);
    else if (rp==1) updatePLRU(idx, r_idx);
    if (op == "w") cache[idx][r_idx].dirty = true;
    else cache[idx][r_idx].dirty = false;
    return out;
}
