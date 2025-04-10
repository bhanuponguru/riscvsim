#include "processor.h"
#include<iostream>
#include<vector>
#include "constants.h"
#include "instr.h"
#include "cache.h"
#include "utils.h"

using namespace std;

void execute(int instruction, long long registers[], char memory[], int& pc, vector<string> lines, vector<int> line_numbers, unordered_map<string, unsigned int> labels, vector<call_item> &call_stack, bool cache_enabled, cache& cache_memory) {
    registers[0]=0; //x0 is 0 for ever.
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
                else if (funct7 == 0x20) { //sub case
                    registers[rd] = registers[rs1] - registers[rs2];
                }
                break;
            case 0x4: // xor case.
                registers[rd] = registers[rs1] ^ registers[rs2];
                break;
            case 0x6:
                if (funct7 == 0x00) { // or case.
                    registers[rd] = registers[rs1] | registers[rs2];
                }
                break;
            case 0x7:
                if (funct7 == 0x00) {// and case.
                    registers[rd] = registers[rs1] & registers[rs2];
                }
                break;
            case 0x1:
                if (funct7 == 0x00) { // sll case.
                    registers[rd] = registers[rs1] << registers[rs2];
                }
                break;
            case 0x5: 
                if (funct7 == 0x00) { // srl case
                    registers[rd] = registers[rs1] >> registers[rs2];
                }
                else if (funct7 == 0x20) { //sra case
                    registers[rd] = registers[rs1] >> registers[rs2];
                }
                break;
            case 0x2:
                if (funct7 == 0x00) { // slt case.
                    registers[rd] = (registers[rs1] < registers[rs2]) ? 1 : 0;
                }
                break;
            case 0x3:
                if (funct7 == 0x00) { // sltu case
                    registers[rd] = ((unsigned long long)registers[rs1] < (unsigned long long)registers[rs2]) ? 1 : 0;
                }
                break;
        }
    }
    else if (opcodes[opcode] == "i") {
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int imm = (instruction>>20)&0b111111111111;
        imm=sign_extend(imm, 12);
        int funct3 = (instruction>>12)&0b111;
        int funct6 =0;
        funct6 |= imm >> 6;
        switch (funct3) {
            case 0x0: // addi case.
                registers[rd] = registers[rs1] + imm;
                break;
            case 0x1: // slli case.
                registers[rd] = registers[rs1] << (imm & 0b111111);
                break;
            case 0x2: // slti case.
                registers[rd] = (registers[rs1] < imm ) ? 1 : 0;
                break;
            case 0x3: // sltiu case.
                registers[rd] = ((unsigned long long)registers[rs1] < (unsigned long long)imm ) ? 1 : 0;
                break;
            case 0x4: // xori case.
                registers[rd] = registers[rs1] ^ imm;
                break;
            case 0x5: 
                if (funct6 == 0x00) { // srli case.
                    registers[rd] = ((unsigned long long)registers[rs1]) >> (imm & 0b111111);
                }
                else if (funct6 == 0x10) { // srai case.
                    registers[rd] = registers[rs1] >> (imm & 0b111111);
                }
                break;
            case 0x6: // ori case.
                registers[rd] = registers[rs1] | imm;
                break;
            case 0x7: // andi case.
                registers[rd] = registers[rs1] & imm;
                break;
        }
    }
    else if(opcodes[opcode] == "jalr") { //J-Type
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int imm = (instruction>>20)&0b111111111111;
        imm=sign_extend(imm, 12);
        registers[rd] = pc + 4;
        pc = registers[rs1] + imm ;
        call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
        if(rs1 == 1) {
            call_stack.pop_back();
        }
        return;
    }
    else if (opcodes[opcode] == "l") { // I-Type
        int rd = (instruction>>7)&0b11111;
        int rs1 = (instruction>>15)&0b11111;
        int imm = (instruction>>20)&0b111111111;
        //shift left and right to sign extend.
        imm=imm<<20;
        imm=imm>>20;
        int funct3 = (instruction>>12)&0b111;
        registers[rd]=0;
        switch (funct3) {
            case 0x0: // lb case.
            if (cache_enabled) {
                char target[1];
                cache_memory.load_memory(registers[rs1]+imm, 1, memory, target);
                registers[rd] = target[0];
            }
            else {
                registers[rd] = memory[registers[rs1]+imm];
            }
                break;
            case 0x1: // lh case.
                // load 2 bytes in little endian format.
                if (cache_enabled) {
                    char target[2];
                    cache_memory.load_memory(registers[rs1]+imm, 2, memory, target);
                    for (int i = 0; i < 2; i++) {
                        registers[rd] |= (target[i] & 0xff) << (i*8);
                    }
                }
                else {
                    for (int i = 0; i < 2; i++) {
                        registers[rd] |= (memory[registers[rs1]+imm+i] & 0xff) << (i*8);
                    }
                }
                registers[rd]=sign_extend(registers[rd], 32);
                break;
            case 0x2: // lw case.
            // load 4 bytes in little endian format.
            if (cache_enabled) {
                char target[4];
                cache_memory.load_memory(registers[rs1]+imm, 4, memory, target);
                for (int i = 0; i < 4; i++) {
                    registers[rd] |= (target[i] & 0xff) << (i*8);
                }
            }
            else {
                for (int i = 0; i < 4; i++) {
                    registers[rd] |= (memory[registers[rs1]+imm+i] & 0xff) << (i*8);
                }
            }
            registers[rd]=sign_extend(registers[rd], 32);
                break;
            case 0x3: // ld case.
            // load 8 bytes in little endian format.
            if (cache_enabled) {
                char target[8];
                cache_memory.load_memory(registers[rs1]+imm, 8, memory, target);
                for (int i = 0; i < 8; i++) {
                    registers[rd] |= (target[i] & 0xff) << (i*8);
                }
            }
            else {
                for (int i = 0; i < 8; i++) {
                    registers[rd] |= (memory[registers[rs1]+imm+i] & 0xff) << (i*8);
                }
            }
                break;
            case 0x4: // lbu case.
            if (cache_enabled) {
                char target[1];
                cache_memory.load_memory(registers[rs1]+imm, 1, memory, target);
                registers[rd] = target[0];
            }
            else {
                registers[rd] = memory[registers[rs1]+imm];
            }
                break;
            case 0x5: // lhu case.
            // load 2 bytes in little endian format.
            if (cache_enabled) {
                char target[2];
                cache_memory.load_memory(registers[rs1]+imm, 2, memory, target);
                for (int i = 0; i < 2; i++) {
                    registers[rd] |= (target[i] & 0xff) << (i*8);
                }
            }
            else {
                for (int i = 0; i < 2; i++) {
                    registers[rd] |= (memory[registers[rs1]+imm+i] & 0xff) << (i*8);
                }
            }
                break;
            case 0x6: // lwu case.
            // load 4 bytes in little endian format.
            if (cache_enabled) {
                char target[4];
                cache_memory.load_memory(registers[rs1]+imm, 4, memory, target);
                for (int i = 0; i < 4; i++) {
                    registers[rd] |= (target[i] & 0xff) << (i*8);
                }
            }
            else {
                for (int i = 0; i < 4; i++) {
                    registers[rd] |= (memory[registers[rs1]+imm+i] & 0xff) << (i*8);
                }
            }
                break;
        }
    }
    else if (opcodes[opcode] == "s") {
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        int imm = ((instruction>>25)&0b1111111)<<5 | ((instruction>>7)&0b11111);
        imm = sign_extend(imm, 12);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // sb case.
            if (cache_enabled) {
                char source[1];
                source[0] = registers[rs2]&0b11111111;
                cache_memory.store_memory(registers[rs1]+imm, 1, memory, source);
            }
            else {
                    memory[registers[rs1]+imm] = registers[rs2]&0b11111111;
            }
                break;
            case 0x1 : // sh case.
                // store 2 bytes in little endian format.
                if (cache_enabled) {
                    char source[2];
                    for (int i = 0; i < 2; i++) {
                        source[i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                    }
                    cache_memory.store_memory(registers[rs1]+imm, 2, memory, source);
                }
                else {
                    for (int i = 0; i < 2; i++) {
                        memory[registers[rs1]+imm+i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                    }
                }
                break;
            case 0x2: // sw case.
            // store 4 bytes in little endian format.
            if (cache_enabled) {
                char source[4];
                for (int i = 0; i < 4; i++) {
                    source[i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                }
                cache_memory.store_memory(registers[rs1]+imm, 4, memory, source);
            }
            else {
                for (int i = 0; i < 4; i++) {
                    memory[registers[rs1]+imm+i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                }
            }
                break;
            case 0x3: // sd case.
            // store 8 bytes in little endian format.
            if (cache_enabled) {
                char source[8];
                for (int i = 0; i < 8; i++) {
                    source[i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                }
                cache_memory.store_memory(registers[rs1]+imm, 8, memory, source);
            }
            else {
                for (int i = 0; i < 8; i++) {
                    memory[registers[rs1]+imm+i] = ((unsigned long long)registers[rs2] >> (i*8)) & 0xff;
                }
            }
                break;
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
        imm = sign_extend(imm, 21);
        registers[rd] = pc + 4;
        pc += imm;
        call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
        if(rd == 1) {
        // to find the label
        for(auto x =labels.begin(); x != labels.end(); x++){
            if(int(x->second) == (pc/4)+1){
                call_stack.push_back(call_item(x->first,line_numbers[current_pc/4] +1));
                break;
            }
        }
        }
        return;
    }
    else if (opcodes[opcode] == "b") {
        int rs1 = (instruction>>15)&0b11111;
        int rs2 = (instruction>>20)&0b11111;
        //in imm, last bit is ignored in the instruction because it is always 0.
        int imm = (((instruction>>31)&0b1)<<12 | ((instruction>>7)&0b1)<<11 | ((instruction>>25)&0b111111)<<5 | ((instruction>>8)&0b1111)<<1) & 0b1111111111110;
        imm = sign_extend(imm, 13);
        int funct3 = (instruction>>12)&0b111;
        switch (funct3) {
            case 0x0: // beq case.
                if (registers[rs1] == registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            case 0x1: // bne case.
                if (registers[rs1] != registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            case 0x4: // blt case.
                if (registers[rs1] < registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            case 0x5: // bge case.
                if (registers[rs1] >= registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            case 0x6: // bltu case.
                if ((unsigned long long)registers[rs1] < (unsigned long long)registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            case 0x7: // bgeu case.
             if ((unsigned long long)registers[rs1] >= (unsigned long long)registers[rs2]) {
                    pc += imm;
                    call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
                    return;
                }
                break;
            
        }
    }
 pc+=4;
 call_stack[call_stack.size()-1].setline(line_numbers[(current_pc/4)] + 1);
}

