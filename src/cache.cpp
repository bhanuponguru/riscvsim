#include<iostream>
#include<string>
#include<fstream>
#include<queue>
#include<list>
#include<unordered_map>
#include<cmath>
#include<random>
#include "cache.h"
#include<algorithm>
#include <sstream>
#include <iomanip>
using namespace std;


block::block() {
    size = 0;
    valid = false;
    dirty = false;
    tag = 0;
}

block::block(size_t s) {
    size = s;
    data.resize(s);
    valid = false;
    dirty = false;
    tag = 0;
}

void block::set_data(char byte, size_t index) {
    data[index] = byte;
}

char block::get_data(size_t index) {
    return data[index];
}

void block::set_valid(bool v) {
    valid = v;
}

bool block::get_valid() {
    return valid;
}

void block::set_dirty(bool d) {
    dirty = d;
}

bool block::get_dirty() {
    return dirty;
}

void block::set_tag(int t) {
    tag = t;
}

unsigned int block::get_tag() {
    return tag;
}



config::config() {
    cache_size = 0;
    block_size = 0;
    associativity = 0;
    replacement_policy = "";
    write_policy = "";
}

config::config(string configfile) {
    ifstream file(configfile);
    file >> cache_size >> block_size >> associativity >> replacement_policy >> write_policy;
    file.close();
}

config cache::get_config() {
    return cache_config;
}

int config::get_cache_size() {
    return cache_size;
}

int config::get_block_size() {
    return block_size;
}

int config::get_associativity() {
    return associativity;
}

string config::get_replacement_policy() {
    return replacement_policy;
}

string config::get_write_policy() {
    return write_policy;
}

string cache::get_output() {
    return output;
}

cache::cache() {
    accesses = 0;
    hits = 0;
}

cache::cache(config c) {
    cache_config = c;
    accesses = 0;
    hits = 0;
    if (c.get_associativity() <= 1) {
        blocks.resize(c.get_cache_size() / c.get_block_size());
        for (int i=0; i<c.get_cache_size() / c.get_block_size(); ++i) {
            blocks[i] = block(c.get_block_size());
        }
    }
    else {
        sets.resize(c.get_cache_size() / c.get_block_size());
        for (int i=0; i<c.get_cache_size() / c.get_block_size(); ++i) {
            sets[i].resize(c.get_associativity());
            for (int j=0; j<c.get_associativity(); ++j) {
                sets[i][j] = block(c.get_block_size());
            }
        }
        if (c.get_replacement_policy() == "fifo") {
            fifo_set.resize(c.get_cache_size() / (c.get_block_size()*c.get_associativity()) );
        } else if (c.get_replacement_policy() == "lru") {
            lru_list_set.resize(c.get_cache_size() / (c.get_block_size()*c.get_associativity()));
        }
    }
}

int cache::get_accesses() {
    return accesses;
}

int cache::get_hits() {
    return hits;
}

void cache::load_memory(int address, size_t nbytes, char* memory, char* target) {
    unsigned int block_size = cache_config.get_block_size();
    unsigned int cache_size = cache_config.get_cache_size();
    unsigned int associativity = cache_config.get_associativity();
    unsigned int num_blocks = cache_size / block_size;
    unsigned int offset_bits = log2(block_size);
    unsigned int offset= address & ((1 << offset_bits) - 1);
    bool Hit = false;
    if (offset + nbytes > block_size) {
        cerr << "Error: Address out of bounds" << endl;
        exit(1);
    }
    ++accesses;
    if (associativity == 0) {
        unsigned int tag=(address >> offset_bits);
        for (size_t i=0; i<num_blocks; i++) {
            if (blocks[i].get_tag() == tag) {
                if (blocks[i].get_valid()) {
                    ++hits;
                    Hit = true;
                    for (size_t j=0; j<nbytes; j++) {
                        target[j] = blocks[i].get_data(offset + j);
                    }
                } else {
                    for (size_t j=0; j<block_size; j++) {
                        blocks[i].set_data(memory[address - offset + j], j);
                    }
                    blocks[i].set_valid(true);
                    blocks[i].set_dirty(false);
                    blocks[i].set_tag(tag);
                    for (size_t j=0; j<nbytes; j++) {
                        target[j] = blocks[i].get_data(offset + j);
                    }
                }
                if (cache_config.get_replacement_policy() == "fifo") {
                    queue<size_t> temp=fifo_full;
                    bool found=false;
                    while (!temp.empty()) {
                        if (temp.front() == i) {
                            temp.pop();
                            found=true;
                            break;
                        }
                        temp.pop();
                    }
                    if (!found) {
                        fifo_full.push(i);
                    }
                }
                else if(cache_config.get_replacement_policy() == "lru") {
                    auto it = find(lru_list.begin(), lru_list.end(),i);
                    if( it!= lru_list.end()) {
                        lru_list.erase(it);
                    }
                    lru_list.push_front(i);
                }
                string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << i  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
                return;
            }
        }
        //in case of cache miss.
        size_t i;
        bool found=false;
        for (size_t j=0; j<num_blocks; j++) {
            if (!blocks[j].get_valid()) {
                i=j;
                found=true;
                break;
            }
        }
        if (!found) {
            if (cache_config.get_replacement_policy() == "fifo") {
                i=fifo_full.front();
                if (!fifo_full.empty()) {
                    fifo_full.pop();
                }
            }
            else if (cache_config.get_replacement_policy() == "random") {
                i = rand() % num_blocks;
            }
            else if (cache_config.get_replacement_policy() == "lru") {
                i = lru_list.back();
                if (!lru_list.empty()) {
                    lru_list.pop_back();
                }
            }
        }
            if (blocks[i].get_dirty()) {
                for (size_t j=0; j<block_size; j++) {
                    memory[(blocks[i].get_tag() << offset_bits) + j] = blocks[i].get_data(j);
                }
            }
            for (size_t j=0; j<block_size; j++) {
                blocks[i].set_data(memory[address - offset + j], j);
            }
            blocks[i].set_valid(true);
            blocks[i].set_dirty(false);
            blocks[i].set_tag(tag);
            for (size_t j=0; j<nbytes; j++) {
                target[j] = blocks[i].get_data(offset + j);
            }
            if (cache_config.get_replacement_policy() == "fifo") {
                fifo_full.push(i);
            }
            else if (cache_config.get_replacement_policy() == "lru") {
                lru_list.push_front(i);
            }
            string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << i  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
            return;
    }
    if (associativity == 1) {
    unsigned int index_bits = log2(num_blocks);
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        if (blocks[index].get_tag() == tag) {
            if (blocks[index].get_valid()) {
                ++hits;
                Hit = true;
                for (size_t j=0; j<nbytes; j++) {
                    target[j] = blocks[index].get_data(offset + j);
                }
            } else {
                for (size_t j=0; j<block_size; j++) {
                    blocks[index].set_data(memory[address - offset + j], j);
                }
                blocks[index].set_valid(true);
                blocks[index].set_dirty(false);
                blocks[index].set_tag(tag);
                for (size_t j=0; j<nbytes; j++) {
                    target[j] = blocks[index].get_data(offset + j);
                }
            }
            string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[index].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
            return;
        }
        //in case of cache miss. there is no need of replacement policy in case of direct mapped cache.
        //in case the bit is dirty, write it back to memory.
        if (blocks[index].get_dirty()) {
            for (size_t j=0; j<block_size; j++) {
                memory[(blocks[index].get_tag() << (offset_bits + index_bits)) + (index << offset_bits) + j] = blocks[index].get_data(j);
            }
        }
        for (size_t j=0; j<block_size; j++) {
            blocks[index].set_data(memory[address - offset + j], j);
        }
        blocks[index].set_valid(true);
        blocks[index].set_dirty(false);
        blocks[index].set_tag(tag);
        for (size_t j=0; j<nbytes; j++) {
            target[j] = blocks[index].get_data(offset + j);
        }
        string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[index].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
        return;
    }
    if (associativity > 1) {
    unsigned int num_sets = num_blocks / associativity;
        unsigned int index_bits = log2(num_sets);
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        for (size_t i=0; i<associativity; i++) {
            if (sets[index][i].get_tag() == tag) {
                if (sets[index][i].get_valid()) {
                    ++hits;
                    Hit = true;
                    for (size_t j=0; j<nbytes; j++) {
                        target[j] = sets[index][i].get_data(offset + j);
                    }
                } else {
                    for (size_t j=0; j<block_size; j++) {
                        sets[index][i].set_data(memory[address - offset + j], j);
                    }
                    sets[index][i].set_valid(true);
                    sets[index][i].set_dirty(false);
                    sets[index][i].set_tag(tag);
                    for (size_t j=0; j<nbytes; j++) {
                        target[j] = sets[index][i].get_data(offset + j);
                    }
                }
                if (cache_config.get_replacement_policy() == "fifo") {
                    queue<size_t> temp=fifo_set[index];
                    bool found=false;
                    while (!temp.empty()) {
                        if (temp.front() == i) {
                            temp.pop();
                            found=true;
                            break;
                        }
                        temp.pop();
                    }
                    if (!found) {
                        fifo_set[index].push(i);
                    }
                }
                else if(cache_config.get_replacement_policy() == "lru") {
                    auto it = find(lru_list_set[index].begin(), lru_list_set[index].end(),i);
                    if( it!= lru_list_set[index].end()) {
                        lru_list_set[index].erase(it);
                    }
                    lru_list_set[index].push_front(i);
                }
                string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( sets[index][i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
                return;
            }
        }
        //in case of cache miss.
        size_t i;
        bool found=false;
        for (size_t j=0; j<associativity; j++) {
            if (!sets[index][j].get_valid()) {
                i=j;
                found=true;
                break;
            }
        }
        if (!found) {
            if (cache_config.get_replacement_policy() == "fifo") {
                i=fifo_set[index].front();
                if (!fifo_set[index].empty()) {
                    fifo_set[index].pop();
                }
            }
            else if(cache_config.get_replacement_policy() == "random") {
                i = rand() % num_blocks;
            }
            else if(cache_config.get_replacement_policy() == "lru") {
                i = lru_list_set[index].back();
                if (!lru_list_set[index].empty()) {
                    lru_list_set[index].pop_back();
                }
            }
        }
            if (sets[index][i].get_dirty()) {
                for (size_t j=0; j<block_size; j++) {
                    memory[(sets[index][i].get_tag() << (offset_bits + index_bits)) + (index << offset_bits) + j] = sets[index][i].get_data(j);
                }
            }
            for (size_t j=0; j<block_size; j++) {
                sets[index][i].set_data(memory[address - offset + j], j);
            }
            sets[index][i].set_valid(true);
            sets[index][i].set_dirty(false);
            sets[index][i].set_tag(tag);
            for (size_t j=0; j<nbytes; j++) {
                target[j] = sets[index][i].get_data(offset + j);
            }
            if (cache_config.get_replacement_policy() == "fifo") {
                fifo_set[index].push(i);
            }
            else if (cache_config.get_replacement_policy() == "lru") {
                lru_list_set[index].push_front(i);
            }
            string temp;
                ostringstream out(temp);
                out << "R: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( sets[index][i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
            return;
    }
}

void cache::store_memory(int address, size_t nbytes, char* memory, char* source) {
    unsigned int block_size = cache_config.get_block_size();
    unsigned int cache_size = cache_config.get_cache_size();
    unsigned int associativity = cache_config.get_associativity();
    unsigned int num_blocks = cache_size / block_size;
    unsigned int offset_bits = log2(block_size);
    unsigned int offset= address & ((1 << offset_bits) - 1);
    bool Hit = false;
    if (offset + nbytes > block_size) {
        cerr << "Error: Address out of bounds" << endl;
        exit(1);
    }
    ++accesses;
    if (associativity == 0) {
        unsigned int tag=(address >> offset_bits);
        for (size_t i=0; i<num_blocks; i++) {
            if (blocks[i].get_tag() == tag) {
                if (blocks[i].get_valid()) {
                    ++hits;
                    Hit = true;
                    if (cache_config.get_write_policy() == "wb") {
                        for (size_t j=0; j<nbytes; j++) {
                            blocks[i].set_data(source[j], offset + j);
                        }
                        blocks[i].set_valid(true);
                        blocks[i].set_dirty(true);
                        blocks[i].set_tag(tag);
                    }
                    else { //write through case.
                        for (size_t j=0; j<block_size; j++) {
                            memory[address - offset + j] = blocks[i].get_data(j);
                        }
                        for (size_t j=0; j<nbytes; j++) {
                            blocks[i].set_data(source[j], offset + j);
                        }
                    }
                }
                else {
                    if (cache_config.get_write_policy() == "wb") {
                        //fetch the latest data from memory.
                        for (size_t j=0; j<block_size; j++) {
                            blocks[i].set_data(memory[(tag << offset_bits) + j], j);
                        }
                    for (size_t j=0; j<nbytes; j++) {
                        blocks[i].set_data(source[j], offset + j);
                    }
                    blocks[i].set_valid(true);
                    blocks[i].set_dirty(true);
                    blocks[i].set_tag(tag);
                    }
                    else { //write through case.
                    //update the memory with source.
                    for (size_t j=0; j<nbytes; j++) {
                        memory[address - offset + j] = source[j];
                    }
                    }
                }
                if (cache_config.get_replacement_policy() == "fifo" && blocks[i].get_valid()) {
                    queue<size_t> temp=fifo_full;
                    bool found=false;
                    while (!temp.empty()) {
                        if (temp.front() == i) {
                            temp.pop();
                            found=true;
                            break;
                        }
                        temp.pop();
                    }
                    if (!found) {
                        fifo_full.push(i);
                    }
                }
                else if(cache_config.get_replacement_policy() == "lru" && blocks[i].get_valid()) {
                    auto it = find(lru_list.begin(), lru_list.end(),i);
                    if( it!= lru_list.end()) {
                        lru_list.erase(it);
                    }
                    lru_list.push_front(i);
                }
                string temp;
                ostringstream out(temp);
                out << "W: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << i  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
                return;
            }
        }
        //in case of cache miss.
        size_t i;
        bool found=false;
        for (size_t j=0; j<num_blocks; j++) {
            if (!blocks[j].get_valid()) {
                i=j;
                found=true;
                break;
            }
        }
        if (!found) {
        if (cache_config.get_replacement_policy() == "fifo") {
            i=fifo_full.front();
        }
        else if(cache_config.get_replacement_policy() == "random") {
            i = rand() % num_blocks;
        }
        else if(cache_config.get_replacement_policy() == "lru") {
            i = lru_list.back();
        }
        }
            if (cache_config.get_write_policy() == "wb") {
                //if the block is dirty, write it back to memory.
                if (blocks[i].get_dirty()) {
                    for (size_t j=0; j<block_size; j++) {
                        memory[(blocks[i].get_tag() << offset_bits) + j] = blocks[i].get_data(j);
                    }
                }
                //fetch the latest data from memory.
                for (size_t j=0; j<block_size; j++) {
                    blocks[i].set_data(memory[(blocks[i].get_tag() << offset_bits) + j], j);
                }
                for (size_t j=0; j<block_size; j++) {
                    blocks[i].set_data(source[j], j);
                }
                blocks[i].set_valid(true);
                blocks[i].set_dirty(true);
                blocks[i].set_tag(tag);
                if (cache_config.get_replacement_policy() == "fifo") {
                    if (!fifo_full.empty() && !found) {
                        fifo_full.pop();
                    }
                    fifo_full.push(i);
                }
                else if (cache_config.get_replacement_policy() == "lru") {
                    if (!lru_list.empty() && !found) {
                        lru_list.pop_back();
                    }
                    lru_list.push_front(i);
                }
            }
            else { //write through case.
            //update the memory with source.
            for (size_t j=0; j<nbytes; j++) {
                memory[address - offset + j] = source[j];
            }
            }
            string temp;
                ostringstream out(temp);
                out << "W: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << i  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();               
                output += temp;
           return;
    }
    if (associativity == 1) {
    unsigned int index_bits = log2(num_blocks);
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        if (blocks[index].get_tag() == tag) {
            if (blocks[index].get_valid()) {
                ++hits;
                Hit = true;
                if (cache_config.get_write_policy() == "wb") {
                    for (size_t j=0; j<nbytes; j++) {
                        blocks[index].set_data(source[j], offset + j);
                    }
                    blocks[index].set_valid(true);
                    blocks[index].set_dirty(true);
                    blocks[index].set_tag(tag);
                }
                else { //write through case.
                    for (size_t j=0; j<nbytes; j++) {
                        blocks[index].set_data(source[j], offset + j);
                    }
                    for (size_t j=0; j<block_size; j++) {
                        memory[address - offset + j] = blocks[index].get_data(j);
                    }
                    blocks[index].set_valid(true);
                    blocks[index].set_dirty(false);
                    blocks[index].set_tag(tag);
                }
            }
            else {
                if (cache_config.get_write_policy() == "wb") {
                //fetch the latest data from memory.
                for (size_t j=0; j<block_size; j++) {
                    blocks[index].set_data(memory[(tag << offset_bits) + j], j);
                }
                //store the source in to the block.
                for (size_t j=0; j<nbytes; j++) {
                    blocks[index].set_data(source[j], offset + j);
                }
                blocks[index].set_valid(true);
                blocks[index].set_dirty(true);
                blocks[index].set_tag(tag);
                }
                else { //write through case.
                //update the memory with source.
                for (size_t j=0; j<nbytes; j++) {
                    memory[address - offset + j] = source[j];
                }
                }
            }
            string temp;
                ostringstream out(temp);
                out << "W: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[index].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
            return;
        }
        //in case of cache miss. there is no need of replacement policy in case of direct mapped cache.
        if (cache_config.get_write_policy() == "wb") {
            if (blocks[index].get_dirty() && blocks[index].get_valid()) {
                for (size_t j=0; j<block_size; j++) {
                    memory[(blocks[index].get_tag() << (offset_bits + index_bits)) + (index << offset_bits) + j] = blocks[index].get_data(j);
                }
            }
            //fetch the latest data from memory.
            for (size_t j=0; j<block_size; j++) {
                blocks[index].set_data(memory[(tag << offset_bits) + j], j);
            }
            for (size_t j=0; j<nbytes; j++) {
                blocks[index].set_data(source[j], offset + j);
            }
            blocks[index].set_valid(true);
            blocks[index].set_dirty(true);
            blocks[index].set_tag(tag);
        }
        else { //write through case.
        //update the memory with source.
        for (size_t j=0; j<nbytes; j++) {
            memory[address - offset + j] = source[j];
        }
        }
        string temp;
                ostringstream out(temp);
                out << "W: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( blocks[index].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
        return;
    }
    if (associativity > 1) {
    unsigned int num_sets = num_blocks / associativity;
        unsigned int index_bits = log2(num_sets);
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        for (size_t i=0; i<associativity; i++) {
            if (sets[index][i].get_tag() == tag) {
                if (sets[index][i].get_valid()) {
                    ++hits;
                    Hit = true;
                    if (cache_config.get_write_policy() == "wb") {
                        for (size_t j=0; j<nbytes; j++) {
                            sets[index][i].set_data(source[j], offset + j);
                        }
                        sets[index][i].set_valid(true);
                        sets[index][i].set_dirty(true);
                        sets[index][i].set_tag(tag);
                    }
                    else { //write through case.
                        for (size_t j=0; j<nbytes; j++) {
                            sets[index][i].set_data(source[j], offset + j);
                        }
                        for (size_t j=0; j<block_size; j++) {
                            memory[address - offset + j] = sets[index][i].get_data(j);
                        }
                        sets[index][i].set_valid(true);
                        sets[index][i].set_dirty(false);
                        sets[index][i].set_tag(tag);
                    }
                }
                else {
                    if (cache_config.get_write_policy() == "wb") {
                    //fetch the latest data from memory.
                    for (size_t j=0; j<block_size; j++) {
                        sets[index][i].set_data(memory[(tag << offset_bits) + j], j);
                    }
                    //store the source in to the block.
                    for (size_t j=0; j<nbytes; j++) {
                        sets[index][i].set_data(source[j], offset + j);
                    }
                    sets[index][i].set_valid(true);
                    sets[index][i].set_dirty(true);
                    sets[index][i].set_tag(tag);
                    }
                    else { //write through case.
                    //update the memory with source.
                    for (size_t j=0; j<nbytes; j++) {
                        memory[address - offset + j] = source[j];
                    }
                    }
                }
                if (cache_config.get_replacement_policy() == "fifo" && sets[index][i].get_valid()) {
                    queue<size_t> temp=fifo_set[index];
                    bool found=false;
                    while (!temp.empty()) {
                        if (temp.front() == i) {
                            temp.pop();
                            found=true;
                            break;
                        }
                        temp.pop();
                    }
                    if (!found) {
                        fifo_set[index].push(i);
                    }
                }
                else if (cache_config.get_replacement_policy() == "lru" && sets[index][i].get_valid()) {
                    auto it = find(lru_list_set[index].begin(), lru_list_set[index].end(),i);
                    if( it!= lru_list_set[index].end()) {
                        lru_list_set[index].erase(it);
                    }
                    lru_list_set[index].push_front(i);
                }
                string temp;
                ostringstream out(temp);
                out << "W: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( sets[index][i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
                return;
            }
        }
        //in case of cache miss.
        size_t i;
        bool found=false;
        for (size_t j=0; j<associativity; j++) {
            if (!sets[index][j].get_valid()) {
                i=j;
                found=true;
                break;
            }
        }
        if (!found) {
        if (cache_config.get_replacement_policy() == "fifo") {
            i=fifo_set[index].front();
        }
        else if(cache_config.get_replacement_policy() == "random") {
            i = rand() % num_blocks;
        } else if(cache_config.get_replacement_policy() == "lru") {
            i = lru_list_set[index].back();
        }
        }
            if (cache_config.get_write_policy() == "wb") {
                if (sets[index][i].get_dirty() && sets[index][i].get_valid()) {
                    for (size_t j=0; j<block_size; j++) {
                        memory[(sets[index][i].get_tag() << (offset_bits + index_bits)) + (index << offset_bits) + j] = sets[index][i].get_data(j);
                    }
                }
                //fetch the latest data from memory.
                for (size_t j=0; j<block_size; j++) {
                    sets[index][i].set_data(memory[(tag << offset_bits) + j], j);
                }
                for (size_t j=0; j<nbytes; j++) {
                    sets[index][i].set_data(source[j], offset + j);
                }
                sets[index][i].set_valid(true);
                sets[index][i].set_dirty(true);
                sets[index][i].set_tag(tag);
                if (cache_config.get_replacement_policy() == "fifo") {
                    if (!fifo_set[index].empty() && !found) {
                        fifo_set[index].pop();
                    }
                    fifo_set[index].push(i);
                }
                else if(cache_config.get_replacement_policy() == "lru" && !found) {
                    if (!lru_list_set[index].empty()) {
                        lru_list_set[index].pop_back();
                    }
                    lru_list_set[index].push_front(i);
                }
            }
            else { //write through case.
            //update the memory with source.
            for (size_t j=0; j<nbytes; j++) {
                memory[address - offset + j] = source[j];
            }
            }
            string temp;
                ostringstream out(temp);
                out << "w: " << "Address: 0x" << hex <<  address <<   ", Set: 0x" << hex << index  << (Hit ? ", Hit" : ", Miss") << ", Tag: 0x" << hex << tag <<( sets[index][i].get_dirty() ? ", Dirty" : ", Clean" ) << endl;
                temp = out.str();
                output += temp;
            return;
    }
}

void cache::dump(string filename) {
    unsigned int associativity = cache_config.get_associativity();
    ofstream output_file(filename);

    if (associativity <= 1) {
        for (size_t i =0; i < blocks.size(); i++) {
            if (blocks[i].get_valid()) {
                output_file << "Set: 0x" << hex << i << "," << "Tag: 0x" << blocks[i].get_tag() << "," << (blocks[i].get_dirty() ? " Dirty" : " Clean") << endl;
            }
        }
    } else {
        for (size_t i = 0; i < sets.size(); i++) {
            for (size_t j = 0; j < associativity; j++) {
                if (sets[i][j].get_valid()) {
                    output_file << "Set: 0x" << hex << i << "," << "Tag: 0x" << sets[i][j].get_tag() << "," << (sets[i][j].get_dirty() ? " Dirty" : " Clean") << endl;
                }
            }
        }
    }
    output_file.close();
}

void cache::clear_cache(char* mem, bool invalidate) {
    unsigned int block_size = cache_config.get_block_size();
    unsigned int cache_size = cache_config.get_cache_size();
    unsigned int associativity = cache_config.get_associativity();
    unsigned int num_blocks = cache_size / block_size;
    if (associativity == 0) {
        unsigned int offset_bits = log2(block_size);
        for (size_t i=0; i<num_blocks; i++) {
            if (blocks[i].get_dirty()) {
                //we wont have index bits in this case.
                for (size_t j=0; j<block_size; j++) {
                    //get address.
                    int address = (blocks[i].get_tag() << offset_bits) + j;
                    mem[address] = blocks[i].get_data(j);
                }
            }
            blocks[i].set_valid(false);
            blocks[i].set_dirty(false);
            blocks[i].set_tag(0);
        }
    }
    if (associativity == 1) {
        unsigned int offset_bits = log2(block_size);
        unsigned int index_bits = log2(num_blocks);
        for (size_t i=0; i<num_blocks; i++) {
            if (blocks[i].get_dirty()) {
                //in this case we also have index bits.
                for (size_t j=0; j<block_size; j++) {
                    int address = (blocks[i].get_tag() << (offset_bits + index_bits)) + (i << offset_bits) + j;
                    mem[address] = blocks[i].get_data(j);
                }
            }
            blocks[i].set_valid(false);
            blocks[i].set_dirty(false);
            blocks[i].set_tag(0);
        }
    }
    if (associativity > 1) {
    unsigned int num_sets = num_blocks / associativity;
    unsigned int offset_bits = log2(block_size);
    unsigned int index_bits = log2(num_sets);
        for (size_t i=0; i<num_sets; i++) {
            for (size_t j=0; j<associativity; j++) {
                if (sets[i][j].get_dirty()) {
                    for (size_t k=0; k<block_size; k++) {
                        int address = (sets[i][j].get_tag() << (offset_bits + index_bits)) + (i << offset_bits) + k;
                        mem[address] = sets[i][j].get_data(k);
                    }
                }
                sets[i][j].set_valid(false);
                sets[i][j].set_dirty(false);
                sets[i][j].set_tag(0);
            }
        }
    }
    if(!invalidate) {
    hits = 0;
    accesses = 0;
    }
    if (cache_config.get_replacement_policy() == "fifo") {
        while (!fifo_full.empty()) {
            fifo_full.pop();
        }
        for (size_t i=0; i<fifo_set.size(); i++) {
            while (!fifo_set[i].empty()) {
                fifo_set[i].pop();
            }
        }
    } else if (cache_config.get_replacement_policy() == "lru") {
        while (!lru_list.empty()) {
            lru_list.pop_back();
        }
        for (size_t i =0; i<lru_list_set.size(); i++) {
            while (!lru_list_set[i].empty()) {
                lru_list_set[i].pop_back();
            }
        }
    }
}