#include "constants.h"

//constants for all types of instructions.
    unordered_map<string, r_instr> r={
        {"add",  r_instr(0b0110011, 0x0, 0x00)},
        {"sub",  r_instr(0b0110011, 0x0, 0x20)},
        {"xor",  r_instr(0b0110011, 0x4, 0x00)},
        {"or",   r_instr(0b0110011, 0x6, 0x00)},
        {"and",  r_instr(0b0110011, 0x7, 0x00)},
        {"sll",  r_instr(0b0110011, 0x1, 0x00)},
        {"srl",  r_instr(0b0110011, 0x5, 0x00)},
        {"sra",  r_instr(0b0110011, 0x5, 0x20)},
        {"slt",  r_instr(0b0110011, 0x2, 0x00)},
        {"sltu", r_instr(0b0110011, 0x3, 0x00)}
        };

    unordered_map<string, i_instr> i={
        {"addi", i_instr(0b0010011, 0x0)},
        {"xori", i_instr(0b0010011, 0x4)},
        {"ori",  i_instr(0b0010011, 0x6)},
        {"andi", i_instr(0b0010011, 0x7)},
        {"slli", i_instr(0b0010011, 0x1)},
        {"srli", i_instr(0b0010011, 0x5)},
        {"srai", i_instr(0b0010011, 0x5)},
        {"slti", i_instr(0b0010011, 0x2)},
        {"sltiu", i_instr(0b0010011, 0x3)},
        //opcode changes from here
        {"lb",  i_instr(0b0000011, 0x0)},
        {"lh",  i_instr(0b0000011, 0x1)},
        {"lw",  i_instr(0b0000011, 0x2)},
        {"ld",  i_instr(0b0000011, 0x3)},
        {"lbu", i_instr(0b0000011, 0x4)},
        {"lhu", i_instr(0b0000011, 0x5)},
        {"lwu", i_instr(0b0000011, 0x6)},
        //opcode changes from here
        {"jalr", i_instr(0b1100111, 0x0)}
    };

    unordered_map<string, s_instr> s={
        {"sb", s_instr(0b0100011, 0x0)},
        {"sh", s_instr(0b0100011, 0x1)},
        {"sw", s_instr(0b0100011, 0x2)},
        {"sd", s_instr(0b0100011, 0x3)}
    };
    unordered_map<string, b_instr> b={
        {"beq", b_instr(0b1100011, 0x0)},
        {"bne", b_instr(0b1100011, 0x1)},
        {"blt", b_instr(0b1100011, 0x4)},
        {"bge", b_instr(0b1100011, 0x5)},
        {"bltu", b_instr(0b1100011, 0x6)},
        {"bgeu", b_instr(0b1100011, 0x7)}
    };

    unordered_map<string, u_instr> u={
        {"lui", u_instr(0b0110111)},
        //opcode changes in U-type
        {"auipc", u_instr(0b0010111)}
    };

    unordered_map<string, j_instr> j={
        {"jal", j_instr(0b1101111)}
    };

    unordered_map<int, string> opcodes={
        {0b0110011, "r"},
{0b0010011, "i"},
//opcode for all load instructions is 0b0000011. although this is i type, but we will will label it as l to differentiate it from other i types.
{0b0000011, "l"},
        {0b1100111, "jalr"},
        {0b0100011, "s"},
        {0b1100011, "b"},
        //we will give seperet labels for lui and auipc because there opcodes are different. we will label them lui and auipc.
        {0b0110111, "lui"},
        {0b0010111, "auipc"},
        {0b1101111, "j"}
    };

unordered_map<string, int> registers;

void initialize_registers() {
    for (int i=0; i<32; ++i) {
        registers["x"+to_string(i)]=i;
    }
    //alias
    registers["zero"] = 0;
    registers["ra"] = 1;
    registers["sp"] = 2;
    registers["gp"] = 3;
    registers["tp"] = 4;
    registers["t0"] = 5;
    registers["t1"] = 6;
    registers["t2"] = 7;
    registers["s0"] = 8;
    registers["s1"] = 9;
    registers["a0"] = 10;
    registers["a1"] = 11;
    registers["a2"] = 12;
    registers["a3"] = 13;
    registers["a4"] = 14;
    registers["a5"] = 15;
    registers["a6"] = 16;
    registers["a7"] = 17;
    registers["s2"] = 18;
    registers["s3"] = 19;
    registers["s4"] = 20;
    registers["s5"] = 21;
    registers["s6"] = 22;
    registers["s7"] = 23;
    registers["s8"] = 24;
    registers["s9"] = 25;
    registers["s10"] = 26;
    registers["s11"] = 27;
    registers["t3"] = 28;
    registers["t4"] = 29;
    registers["t5"] = 30;
    registers["t6"] = 31;
}