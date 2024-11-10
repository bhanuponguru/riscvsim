#include<iostream>
#include<string>
#include<fstream>
#include<queue>
#include<list>
#include<unordered_map>
#include<cmath>
#include "cache.h"


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
            fifo_set.resize(c.get_cache_size() / c.get_block_size());
        }
        else if (c.get_replacement_policy() == "lru") {
            lru_set.resize(c.get_cache_size() / c.get_block_size());
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
    unsigned int num_sets = num_blocks / associativity;
    unsigned int offset_bits = log2(block_size);
    unsigned int index_bits = log2(num_sets);
    unsigned int offset= address & ((1 << offset_bits) - 1);
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
                    fifo_full.push(i);
                }
                return;
            }
        }
        //in case of cache miss.
        if (cache_config.get_replacement_policy() == "fifo") {
            size_t i=fifo_full.front();
            fifo_full.pop();
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
            fifo_full.push(i);
            return;
        }
    }
    if (associativity == 1) {
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        if (blocks[index].get_tag() == tag) {
            if (blocks[index].get_valid()) {
                ++hits;
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
        return;
    }
}

void cache::store_memory(int address, size_t nbytes, char* memory, char* source) {
    unsigned int block_size = cache_config.get_block_size();
    unsigned int cache_size = cache_config.get_cache_size();
    unsigned int associativity = cache_config.get_associativity();
    unsigned int num_blocks = cache_size / block_size;
    unsigned int num_sets = num_blocks / associativity;
    unsigned int offset_bits = log2(block_size);
    unsigned int index_bits = log2(num_sets);
    unsigned int offset= address & ((1 << offset_bits) - 1);
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
                    fifo_full.push(i);
                }
                return;
            }
        }
        //in case of cache miss.
        if (cache_config.get_replacement_policy() == "fifo") {
            size_t i=fifo_full.front();
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
                fifo_full.pop();
                fifo_full.push(i);
            }
            else { //write through case.
            //update the memory with source.
            for (size_t j=0; j<nbytes; j++) {
                memory[address - offset + j] = source[j];
            }
            }
           return;
        }
    }
    if (associativity == 1) {
        unsigned int index=(address >> offset_bits) & ((1 << index_bits) - 1);
        unsigned int tag=(address >> (offset_bits + index_bits));
        if (blocks[index].get_tag() == tag) {
            if (blocks[index].get_valid()) {
                ++hits;
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
        return;
    }
}


void cache::clear_cache() {
    unsigned int block_size = cache_config.get_block_size();
    unsigned int cache_size = cache_config.get_cache_size();
    unsigned int associativity = cache_config.get_associativity();
    unsigned int num_blocks = cache_size / block_size;
    unsigned int num_sets = num_blocks / associativity;
    if (associativity == 0) {
        for (size_t i=0; i<num_blocks; i++) {
            blocks[i].set_valid(false);
            blocks[i].set_dirty(false);
            blocks[i].set_tag(0);
        }
    }
    if (associativity == 1) {
        for (size_t i=0; i<num_sets; i++) {
            blocks[i].set_valid(false);
            blocks[i].set_dirty(false);
            blocks[i].set_tag(0);
        }
    }
    if (associativity > 1) {
        for (size_t i=0; i<num_sets; i++) {
            for (size_t j=0; j<associativity; j++) {
                sets[i][j].set_valid(false);
                sets[i][j].set_dirty(false);
                sets[i][j].set_tag(0);
            }
        }
    }
}