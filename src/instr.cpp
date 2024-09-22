#include "instr.h"
#include<iostream>
#include<string>
#include "constants.h"
using namespace std;

r_instr::r_instr() : opcode(0), funct3(0), funct7(0) {}
r_instr::r_instr(int oc, int f3, int f7) : opcode(oc), funct3(f3), funct7(f7) {}

i_instr::i_instr() : opcode(0), funct3(0) {}
i_instr::i_instr(int oc, int f3) : opcode(oc), funct3(f3) {}

s_instr::s_instr() : opcode(0), funct3(0) {}
s_instr::s_instr(int oc, int f3) : opcode(oc), funct3(f3) {}

b_instr::b_instr() : opcode(0), funct3(0) {}
b_instr::b_instr(int oc, int f3) : opcode(oc), funct3(f3) {}

u_instr::u_instr() : opcode(0) {}
u_instr::u_instr(int oc) : opcode(oc) {}

j_instr::j_instr() : opcode(0) {}
j_instr::j_instr(int oc) : opcode(oc) {}

int r_instr::generate_instr(int rd, int rs1, int rs2) {
    int instr = 0;
    instr |= opcode;
    instr |= rd<<7;
    instr |= funct3<<12;
    instr |= rs1<<15;
    instr |= rs2<<20;
    instr |= funct7<<25;
    return instr;
}

int i_instr::generate_instr(int rd, int rs1, int imm){
    int instr = 0;
    instr |= opcode;
    instr |= rd<<7;
    instr |= funct3<<12;
    instr |= rs1<<15;
    instr |= imm<<20;
    return instr;
}

int s_instr::generate_instr(int rs1, int rs2, int imm){
        int imm4_0 = imm & 0b11111;
    int imm11_5 = (imm >> 5) & 0b1111111;
    int instr = 0;
    instr |= opcode;
    instr |= imm4_0 << 7;
    instr |= funct3 << 12;
    instr |= rs1 << 15;
    instr |= rs2 << 20;
    instr |= imm11_5 << 25;
    return instr;
}

int b_instr::generate_instr(int rs1, int rs2, int imm) {
    int instr=0;
    instr |= opcode;
    int imm_4_1_11 = 0 | ((imm>>11)&1);
    imm_4_1_11|=imm & 0b11110; //4:1 bits. we dont need bit 0.
    instr |= imm_4_1_11<<7;
    instr |= funct3<<12;
    instr |= rs1<<15;
    instr |= rs2<<20;
    int imm_12_10_5 = 0 | ((imm>>5) & 0b111111); //10 to 5 bits.
    int t = (imm>>12)&0b1; //only 12th bit.
    imm_12_10_5 |= t<<6;
    instr |= imm_12_10_5 <<25;
    return instr;
}

int u_instr::generate_instr(int rd, int imm) {
    int instr=0;
    instr |= opcode;
    instr |= rd<<7;
    instr |= imm<<12;
    return instr;
}

int j_instr::generate_instr(int rd, int imm) {
    int instr = 0;
    int imm10_1 = imm & 0b1111111111;
    int imm11 = (imm >> 11) & 0b1;
    int imm19_12 = (imm >>12) & 0b11111111;
    int imm20 = (imm >>20) & 0b1;
    instr |= opcode;
    instr |= rd << 7;
    instr |= imm19_12 <<12;
    instr |= imm11 << 20;
    instr |= imm10_1 << 21;
    instr |= imm20 <<31;
    return instr;
}

string r_instr::get_operation(int opcode, int funct3, int funct7) {
    if (opcode == 0b0110011 && funct3 == 0b000 && funct7 == 0b0000000) {
        return "add";
    }
    return "invalid";
}

string i_instr::get_operation(int opcode, int funct3) {
    if (opcode == 0b0000011 && funct3 == 0b000) {
        return "lb";
    }
    return "invalid";
}

string s_instr::get_operation(int opcode, int funct3) {
    if (opcode == 0b0100011 && funct3 == 0b000) {
        return "sb";
    }
    return "invalid";
}

string b_instr::get_operation(int opcode, int funct3) {
    if (opcode == 0b1100011 && funct3 == 0b000) {
        return "beq";
    }
    return "invalid";
}

string u_instr::get_operation(int opcode) {
    if (opcode == 0b0110111) {
        return "lui";
    }
    return "invalid";
}

string j_instr::get_operation(int opcode) {
    if (opcode == 0b1101111) {
        return "jal";
    }
    return "invalid";
}

