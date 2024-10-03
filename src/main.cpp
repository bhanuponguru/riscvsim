#include<iostream>
#include<iomanip>
#include<vector>
#include<string>
#include<fstream>
#include<unordered_map>
#include "instr.h"
#include "utils.h"
#include "constants.h"
#include "preprocessor.h"
#include "processor.h"

using namespace std;




int main(int argc, char *argv[]) {
    char mem[0x50000]={0}; //we initialized with char because we want to access memory byte by byte.
    long long regs[32]={0};
    int pc=0;
    vector<string> lines;
    vector<int> line_numbers;
   unordered_map<string, unsigned int> labels; // a hashmap to store  labels and there line numbers.
     //initialize stack pointer register.
    vector<int> break_line;
    regs[2]=0x50000;
    vector<call_item> call_stack; //we are using vector because we have push pop functionality and we can access all the elements.
     initialize_registers();
     while (true) { //an infinite loop to take commands from the user.
        string command;
        cout << "riscvsim> ";
        cin >> command;
        if(*(int*)(mem+pc) == 0 || pc >= 0x10000){
            call_stack.clear();
        }
        if (command == "load") {
            string filename;
            cin >> filename;
            load_from_file(filename, mem, regs, pc, labels, line_numbers, lines);
            call_stack.push_back(call_item("main",0));
        }
        else if (command == "run") {
            int prev_pc = pc;
            while (pc < 0x10000 && *(int*)(mem+pc) != 0) {
                int instr=*(int*)(mem+pc);
                execute(instr, regs, mem, pc, lines, line_numbers,labels,call_stack);
                cout << "executed " << get_instr(lines[line_numbers[(prev_pc/4)]]) << "; pc=0x" << int_to_hex(prev_pc,8) << endl;
                prev_pc = pc;
                //check if any current line after execution matches any breakpoint
                size_t i;
               for (i=0; i < break_line.size(); i++ ){
                if(line_numbers[pc/4] + 1 == break_line[i] ){
                    break;
                }
               }
               if(i< break_line.size()) {
                cout << "Execution stopped at breakpoint" << endl;
                break;
               }
            }
        }
        else if (command == "step") {
            //if no instructions, then report that execution completed.
            if (pc >= 0x10000 || *(int*)(mem+pc) == 0) {
                cout << "execution completed." << endl;
                continue;
            }
            int instr=*(int*)(mem+pc);
            int prev_pc = pc;
            execute(instr, regs, mem, pc, lines, line_numbers, labels, call_stack);
            cout << "executed " << get_instr(lines[line_numbers[(prev_pc/4)]]) << "; pc=0x" << int_to_hex(prev_pc,8) << endl;
        }
        else if (command == "regs") {
            for (int i=0; i<32; ++i) {
                cout << "x" << i << ": 0x" << int_to_hex(regs[i], 16) << endl;
            }
        }
        else if (command == "reg") {
            int reg;
            cin >> reg;
            cout << "x" << reg << ": 0x" << int_to_hex(regs[reg], 16) << endl;
        }
        else if (command == "mem") {
            size_t base, size;
            cin >> hex >> base;
            cin >> size;
            //print memory starting from base to base+size.
            for (size_t i=base; i<base+size; ++i) {
                cout << "memory at 0x" << hex << i << ": 0x" << hex << setw(2) << (int)(unsigned char)mem[i] << endl;
            }
        }
        else if (command == "break") {
                if(break_line.size() >=5) {
                    cout << "Maximum Number of Breakpoints reached" << endl;
                    continue;
                }
                int line_number;
                cin >> line_number;

                break_line.push_back(line_number);

                cout << "Breakpoint Set at line number " << line_number << endl;
        }
        else if (command == "delete_break") {
            int line_number;
            cin >> line_number;

            //To find and delete breakpoint.
            if(break_line.empty()){
                cout << "No breakpoint set at line number " << line_number << endl;               
            }
            else{
                size_t i;
            for (i =0; i < break_line.size(); i++) {
                if(break_line[i] == line_number) {
                    break_line.erase(break_line.begin() + i);
                    cout << "Deleted breakpoint at line number " << line_number << endl;
                }
            }
            if(i == break_line.size()) {
                cout << "No breakpoint set at line number " << line_number << endl;
                }
            }
        }
        else if(command == "show-stack") {
            if(call_stack.empty()) {
                cout << "Empty call stack : Execution Complete" << endl;
            }
            else {
                cout << "Call stack:" << endl;
            for(size_t i=0; i<call_stack.size(); i++) {
                    cout << call_stack[i].get_label() << ":" << call_stack[i].get_line() << endl; 
                }
            }
        }

        else if (command == "exit") {
            cout << "Exited the Simulator" << endl;
            break;
        }
        else {
            cout << "Invalid command" << endl;
        }
        cout << endl;
     }
}