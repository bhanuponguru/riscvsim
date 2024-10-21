#pragma once
#include<iostream>
#include<string>
using namespace std;

class r_instr {
    //required fields for r type instructions, that is opcode, funct3 and funct7.
    int opcode;
    int funct3;
    int funct7;
    public:
    r_instr(); //default constructor.

    r_instr(int oc, int f3, int f7);
    int generate_instr(int rd, int rs1, int rs2);
    string get_operation(int opcode, int funct3, int funct7);
};

class i_instr{
    // required fields for i type instructions, that is opcode, funct3.
    int opcode;
    int funct3;
    public:
    i_instr(); // default constructor.

    i_instr(int oc, int f3);
    int generate_instr(int rd, int rs1, int imm); // imm = immediate value    
    string get_operation(int opcode, int funct3);
};

class s_instr{
    //required fields for s type instructions, that is opcode, funct3.
    int opcode;
    int funct3;
    public:
    s_instr(); // default constructor.

    s_instr(int oc, int f3);
    int generate_instr(int rs1, int rs2, int imm);
    string get_operation(int opcode, int funct3);
};

class b_instr {
    //required fields for b type instructions, that is opcode, funct3.
    int opcode;
    int funct3;
    public:
    b_instr(); //default constructor.

    b_instr(int oc, int f3);
    int generate_instr(int rs1, int rs2, int imm);
    string get_operation(int opcode, int funct3);
};


class u_instr {
    //required fields for u type instructions, that is opcode.
    int opcode;
    public:
    u_instr(); //default constructor.

    u_instr(int oc);
    int generate_instr(int rd, int imm);
    string get_operation(int opcode);
};

class j_instr {
    //required fields for j type instructions, that is opcode.
    int opcode;
    public:
    j_instr(); //default constructor.

    j_instr(int oc);
    int generate_instr(int rd, int imm);
    string get_operation(int opcode);
};