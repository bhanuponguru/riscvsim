#pragma once
#include "constants.h"
#include<vector>

void execute(int instruction, long long registers[], char memory[], int& pc, vector<string> lines, vector<int> line_numbers); //function to execute the instruction. It takes the instruction, registers, memory, pc, lines and line_numbers as arguments, and updates the memory and/or registers, then prints the line and pc.