#pragma once
#include<string>
#include<variant>
#include<vector>
#include<queue>
#include<list>
#include<unordered_map>

using namespace std;

class block {
    vector<char> data;
    size_t size;
    bool valid;
    bool dirty;
    unsigned int tag;
public:
block();
block(size_t s);
void set_data(char byte, size_t index);
char get_data(size_t index);
void set_valid(bool v);
bool get_valid();
void set_dirty(bool d);
bool get_dirty();
void set_tag(int t);
unsigned int get_tag();
};


class config {
    int cache_size;
    int block_size;
    int associativity;
    string replacement_policy;
    string write_policy;
public:
config();
config(string configfile);
int get_cache_size();
int get_block_size();
int get_associativity();
string get_replacement_policy();
string get_write_policy();
};

class cache {
    vector<block> blocks;
    vector<vector<block>> sets;
    config cache_config;
queue<size_t> fifo_full; //this is to store blocks in fully associative cache in case of fifo. this stores index to the block.
vector<queue<block*>> fifo_set; //this is to store blocks in set associative cache in case of fifo.
unordered_map<block*, list<block*>::iterator> lru_full; //this is to store blocks in fully associative cache in case of lru.
vector<unordered_map<block*, list<block*>::iterator>> lru_set; //this is to store blocks in set associative cache in case of lru.
    int accesses;
    int hits;
public:
cache();
cache(config c);
void load_memory(int address, size_t nbytes, char* memory, char* target);
void store_memory(int address, size_t nbytes, char* memory, char* source);
int get_accesses();
int get_hits();
};
