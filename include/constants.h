#include<unordered_map>
#include<string>
#include "instr.h"

using namespace std;

extern unordered_map<string, r_instr> r;
extern unordered_map<string, b_instr> b;
extern unordered_map<string, u_instr> u;
extern unordered_map<string, i_instr> i;
extern unordered_map<string, s_instr> s;
extern unordered_map<string, j_instr> j;
extern unordered_map<string, int> registers;
extern unordered_map<int, string> opcodes;
void initialize_registers();