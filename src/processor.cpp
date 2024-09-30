#include "processor.h"
#include<iostream>
#include<vector>
#include "constants.h"
#include "instr.h"
#include "utils.h"

using namespace std;

void execute(int instruction, long long registers[], char memory[], int& pc, vector<string> lines, vector<int> line_numbers) {
    int current_pc=pc;
    int opcode = instruction&0b1111111;
    if (opcodes[opcode] == "r") {
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        int funct3 = (instruction>>12)&0b111;
        int funct7 = (instruction>>25)&0b1111111;
        switch (funct3) {
            case 0x0:
                if (funct7 == 0x00) { // add case.
                    registers[rd] = registers[rs1] + registers[rs2];
                }
                else if (funct7 == 0x20) {
                    registers[rd] = registers[rs1] - registers[rs2];
                }
                break;
            case 0x4: // xor case.
                registers[rd] = registers[rs1] ^ registers[rs2];
                break;
        }
    }
    else if (opcodes[opcode] == "i") {
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int imm = (instruction>>20)&0b111111111111;
        imm=sign_extend(imm, 12);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // addi case.
                registers[rd] = registers[rs1] + imm;
                break;
        }
    }
    else if (opcodes[opcode] == "l") {
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int imm = (instruction>>20)&0b111111111;
        //shift left and right to sign extend.
        imm=imm<<20;
        imm=imm>>20;
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // lb case.
                registers[rd] = memory[registers[rs1]+imm];
                break;
                case 0x1: // lh case.
                // load 2 bytes in little endian format.
                registers[rd] = memory[registers[rs1]+imm] | (memory[registers[rs1]+imm+1]<<8);
                break;
        }
    }
    else if (opcodes[opcode] == "s") {
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        int imm = ((instruction>>25)&0b1111111)<<5 | ((instruction>>7)&0b11111);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // sb case.
                memory[registers[rs1]+imm] = registers[rs2]&0b11111111;
                break;
                case 0x1 : // sh case.
                // store 2 bytes in little endian format.
                memory[registers[rs1]+imm] = registers[rs2]&0b11111111;
                memory[registers[rs1]+imm+1] = (registers[rs2]>>8)&0b11111111;
        }
    }
    else if (opcodes[opcode] == "lui") {
        int rd = (instruction>>7)&0b11111;
        int imm = (instruction>>12);
        registers[rd] = imm << 12;
    }
    else if (opcodes[opcode] == "auipc") {
        int rd = (instruction>>7)&0b11111;
        int imm = (instruction>>12);
        registers[rd] = pc + (imm<<12);
    }
    else if (opcodes[opcode] == "j") {
        int rd = (instruction>>7)&0b11111;
        int imm = ((instruction>>31)&0b1)<<20 | ((instruction>>12)&0b11111111)<<12 | ((instruction>>20)&0b1)<<11 | ((instruction>>21)&0b11111111111)<<1;
        registers[rd] = pc + 4;
        pc += imm;
        return;
    }
    else if (opcodes[opcode] == "b") {
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        //in imm, last bit is ignored in the instruction because it is always 0.
        int imm = (((instruction>>31)&0b1)<<12 | ((instruction>>7)&0b1)<<11 | ((instruction>>25)&0b111111)<<5 | ((instruction>>8)&0b1111)<<1) & 0b1111111111110;
        imm=sign_extend(imm, 12);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // beq case.
                if (registers[rs1] == registers[rs2]) {
                    pc += imm;
 cout << "executed instruction: " << lines[line_numbers[(current_pc/4)]] << " ; under pc: " << current_pc << " in line " << (current_pc/4)+1 << endl;
                    return;
                }
                break;
        }
    }
 pc+=4;
 cout << "executed instruction: " << lines[line_numbers[(current_pc/4)]] << " ; under pc: " << current_pc << " in line " << (current_pc/4)+1 << endl;
}

