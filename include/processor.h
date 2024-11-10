#pragma once
#include "constants.h"
#include<vector>
#include  "utils.h"
#include  "cache.h"

void execute(int instruction, long long registers[], char memory[], int& pc, vector<string> lines, vector<int> line_numbers,unordered_map<string,unsigned int> labels, vector<call_item> &call_stack, bool cache_enabled,  cache& cache_memory);
