#include "processor.h"
#include "constants.h"
#include "instr.h"

void execute(int instruction, long long registers[], char memory[], int& pc) {
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
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // addi case.
                registers[rd] = registers[rs1] + imm;
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
    else if (opcodes[opcode] == "b") {
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        int imm = ((instruction>>31)&0b1)<<12 | ((instruction>>25)&0b111111)<<5 | ((instruction>>8)&0b1111)<<1 | ((instruction>>7)&0b1);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // beq case.
                if (registers[rs1] == registers[rs2]) {
                    pc += imm;
                }
                break;
        }
    }
 pc=pc+4;
}

