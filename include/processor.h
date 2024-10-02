#pragma once
#include "constants.h"
#include<vector>
#include <utils.h>
void execute(int instruction, long long registers[], char memory[], int& pc, vector<string> lines, vector<int> line_numbers,unordered_map<string,unsigned int> labels, vector<call_item> &call_stack); //function to execute the instruction. It takes the instruction, registers, memory, pc, lines and line_numbers as arguments, and updates the memory and/or registers, then prints the line and pc.