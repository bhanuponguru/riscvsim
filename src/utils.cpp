#include "utils.h"
#include<algorithm>
#include<sstream>
#include<iostream>
#include<fstream>
#include<unordered_map>
#include<string>
#include<vector>
#include "constants.h"
#include "preprocessor.h"
#include "instr.h"

using namespace std;
string int_to_hex(long long n, unsigned int count, bool sign_extend) {
    stringstream ss;
    ss << hex << n;
    string res=ss.str();
    //if count is 0, we return the whole string.
    if (count == 0) {
        return res;
    }
    // if the number is less than count digits, we need to add leading zeros.
    if (res.size() < count) {
        res=string(count-res.size(), sign_extend ? res[0] : '0')+res;
    }
    // if the number is more than count digits, we need to remove the leading digits.
    if (res.size() > count) {
        res=res.substr(res.size()-count);
    }
    return res;
}

int sign_extend(int num, int bits) {
    num=num<<(32-bits);
    num=num>>(32-bits);
    return num;
}

long long to_int(string num) {
    if (num[0] == '0' && num[1] == 'x') {
        return stoll(num, nullptr, 16);
    }
    else if (num[0] == '0' && num[1] == 'b') {
        return stoll(num, nullptr, 2);
    }
    else {
        return stoll(num);
    }
    return stoll(num);
}

int get_label(string s, int x, unordered_map<string, unsigned int> labels) {
    if (labels.find(s) != labels.end()) {
        return labels[s];
    }
    cerr << "Error: " << s << " is not a valid label at line " << x << "." << endl;
    exit(1);
}

int get_register(string s, int x) {
    if (registers.find(s) != registers.end()) {
        return registers[s];
    }
    cerr << "Error: " << s << " is not a valid register at line " << x << "." << endl;
    exit(1);
}

void load_from_file(string filename, char memory[], long long registers[], int& pc, unordered_map<string, unsigned int>& labels, vector<int>& line_numbers, vector<string>& lines) {
    ifstream input_file(filename);
    labels.clear();
    line_numbers.clear();
    lines.clear();
    string line;
    while (getline(input_file, line)) {
        lines.push_back(trim_space(line));
    }
    //clear labels, line numbers, lines, and every vector.
    //clear all registers.
    for (int i=0; i<32; ++i) {
        registers[i]=0;
    }
    //clear the memory.
    for (int i=0; i<0x50000; ++i) {
        memory[i]=0;
    }
    //initialize stack pointer register.
    pc=0; //reset pc.
    vector<vector<string>> instructions=preprocess_and_parse(lines, labels, line_numbers, memory+0x10000);
        for (size_t x=0; x < instructions.size(); ++x) {
        if (instructions[x].size() == 0) {
            continue;
        }
        string operation=instructions[x][0];
           if (r.find(operation) != r.end()) {
            if (instructions[x].size() != 4) {
                cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                exit(1);
            }
            r_instr instr=r[operation];
             int rd=get_register(instructions[x][1], line_numbers[x]+1);
            int rs1=get_register(instructions[x][2], line_numbers[x]+1);
            int rs2=get_register(instructions[x][3], line_numbers[x]+1);
            *(unsigned int*)(memory+4*x)=instr.generate_instr(rd,rs1,rs2);
            continue;
                }

            if(i.find(operation) != i.end()){
                if (instructions[x].size() != 4) {
                    cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                    exit(1);
                }            
                i_instr instr=i[operation];
                int rd = get_register(instructions[x][1], line_numbers[x]+1);
                int rs1 = get_register(instructions[x][2], line_numbers[x]+1);
                int imm;
                    imm = to_int(instructions[x][3]);
               if (operation == "slli" || operation == "srli" || operation == "srai")
               {
                    if(imm<0 || imm > 63){
                       cerr << " immediate value " << imm << " does not fit in 6 bits for the " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                        exit(1);
                    }    
               }
               else {
                imm=sign_extend(imm,12);
                if(imm< -2048 || imm >2047){
                  cerr << " immediate value " << imm << " does not fit in 12 bits for the " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                    exit(1);
                }
               }
               
                 //if these are shift instructions, some of them have funct combined with imm.
                if (operation == "slli") {
                    imm = imm & 0b111111;
                    imm |= 0x00 << 6;
                }
                if (operation == "srli") {
                    imm = imm & 0b111111;
                    imm |= 0x00 << 6;                                                                                                      
                }
                if (operation == "srai") {
                    imm = imm & 0b111111;
                    imm |= 0x10 << 6;
                }
            *(unsigned int*)(memory+4*x)=instr.generate_instr(rd,rs1,imm);
                continue;
                }

            if (s.find(operation) != s.end()){
                if (instructions[x].size() != 4) {
                    cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                    exit(1);
                }
                s_instr instr=s[operation];
                int rs1 = get_register(instructions[x][2], line_numbers[x]+1);
                int rs2 = get_register(instructions[x][1], line_numbers[x]+1);
                int imm = to_int(instructions[x][3]);
                if( imm < -2048 || imm > 2047) {
                  cerr << " immediate value " << imm << " does not fit in 12 bits for the " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                    exit(1);
                }
                *(unsigned int*)(memory+4*x)=instr.generate_instr(rs1,rs2,imm);
                continue;
                }
                        
        
        if (b.find(operation) != b.end()) {
            if (instructions[x].size() != 4) {
                cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                exit(1);
            }
            b_instr instr=b[operation];
            int rs1=get_register(instructions[x][1], line_numbers[x]+1);
            int rs2=get_register(instructions[x][2], line_numbers[x]+1);
            int imm=(get_label(instructions[x][3], line_numbers[x]+1, labels)-x-1)*4;
            *(unsigned int*)(memory+4*x)=instr.generate_instr(rs1,rs2,imm);
            continue;
        }
        if (u.find(operation) != u.end()) {
            if (instructions[x].size() != 3) {
                cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                exit(1);
            }
            u_instr instr=u[operation];
            int rd=get_register(instructions[x][1], line_numbers[x]+1);
            int imm=to_int(instructions[x][2]);
               if(imm < 0 || imm > 4294967295) {
                cerr << " immediate value " << imm << " does not fit in 32 bits for the " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                exit(1);
            }
            *(unsigned int*)(memory+4*x)=instr.generate_instr(rd,imm);
            continue;
            }
        if (j.find(operation) != j.end()) {
            if (instructions[x].size() != 3) {
                cerr << "invalid number of arguments for " << operation << " at line " << line_numbers[x]+1 << "." << endl;
                exit(1);
            }
            j_instr instr=j[operation];
            int rd=get_register(instructions[x][1], line_numbers[x]+1);
        int imm=(get_label(instructions[x][2], line_numbers[x]+1, labels)-x-1)*2;
        *(unsigned int*)(memory+4*x)=instr.generate_instr(rd,imm);
            continue;
        }
          cerr << "invalid operation " << operation << " at line " << line_numbers[x]+1 << "." << endl;
        exit(1);
     }
input_file.close();
}


call_item::call_item(string label, int line) {
    this->label=label;
        this->line=line;
}

string call_item::get_label() {
    return label;
}

int call_item::get_line() {
    return line;
}

void call_item::setline(int line) {
    this->line=line;
}

string get_instr(string line) {
    //if there is a colon, then the instruction is after the colon and space. or else return same line.
    size_t pos=0;
    for (pos=0; pos < line.length(); ++pos) {
        if (line[pos] == ':') {
            break;
        }
    }
    if (pos == line.length()) {
        return trim_space(line);
    }
    return trim_space(line.substr(pos+2));
}

string trim_space(string s) {
    size_t start=0;
    size_t end=s.length();
    while (start < s.length() && (s[start] == ' ' || s[start] == '\t')) {
        start++;
    }
    while (end > 0 && (s[end-1] == ' ' || s[end-1] == '\t')) {
        end--;
    }
    return s.substr(start, end-start);
}