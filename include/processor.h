#pragma once
#include "constants.h"

void execute(int instruction, long long registers[], char memory[], int& pc); //takes instruction, registers and memory as input and executes the instruction, updating the registers and memory accordingly.
