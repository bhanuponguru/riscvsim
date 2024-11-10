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
#include "cache.h"

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
    vector<call_item> call_stack; //we are using vector because we have push pop functionality and we can access all the elements.
     initialize_registers();
        cache cache_memory;
        bool cache_enabled=false;
        bool executing=false;
     while (true) { //an infinite loop to take commands from the user.
     regs[0]=0; //x0 is 0 for ever.
        string command;
        cin >> command;
        if(*(int*)(mem+pc) == 0 || pc >= 0x10000){
            call_stack.clear();
            executing=false;
        }
        if (command == "load") {
            string filename;
            cin >> filename;
            load_from_file(filename, mem, regs, pc, labels, line_numbers, lines);
            break_line.clear();
            call_stack.push_back(call_item("main",0));
            executing=false;
            if (cache_enabled) {
                cache_memory.clear_cache();
            }
        }
        else if (command == "cache") {
            string cmd;
            cin >> cmd;
                if (cmd == "disable") {
            if (executing) {
                cout << "Cannot change cache configuration while executing" << endl;
                continue;
            }
                    cache_enabled=false;
                    cout << "Cache disabled" << endl;
                }
                else if (cmd == "enable") {
            if (executing) {
                cout << "Cannot change cache configuration while executing" << endl;
                continue;
            }
                    cache_enabled=true;
                string configfile;
                cin >> configfile;
                config cache_config=config(configfile);
                cache_memory=cache(cache_config);
                    cout << "Cache enabled" << endl;
                }
                else if (cmd == "status") {
                    //display number of accesses and hits.
                    cout << "Accesses: " << cache_memory.get_accesses() << ". Hits: " << cache_memory.get_hits() << endl;
                }
        }
        else if (command == "run") {
            if (pc >= 0x10000 || *(int*)(mem+pc) == 0) {
                cout << "Nothing to run" << endl;
                continue;
            }
            int prev_pc = pc;
            while (pc < 0x10000 && *(int*)(mem+pc) != 0) {
                if (!executing) {
                    executing = true;
                }
                int instr=*(int*)(mem+pc);
                execute(instr, regs, mem, pc, lines, line_numbers,labels,call_stack,cache_enabled, cache_memory);
                cout << "Executed " << get_instr(lines[line_numbers[(prev_pc/4)]]) << "; PC=0x" << int_to_hex(prev_pc,8) << endl;
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
                cout << "Nothing to step" << endl;
                continue;
            }
            if (!executing) {
                executing = true;
            }
            int instr=*(int*)(mem+pc);
            int prev_pc = pc;
            execute(instr, regs, mem, pc, lines, line_numbers, labels, call_stack, cache_enabled, cache_memory);
            cout << "executed " << get_instr(lines[line_numbers[(prev_pc/4)]]) << "; PC=0x" << int_to_hex(prev_pc,8) << endl;
        }
        else if (command == "regs") {
            cout << "Registers:" << endl;
            for (int i=0; i<32; ++i) {
                cout << "x" << dec << i;
                if (i < 10) {
                    cout << " ";
                }
                cout << " = 0x" << hex << regs[i] << endl;
            }
        }
        else if (command == "mem") {
            size_t base, size;
            cin >> hex >> base;
            cin >> dec >> size;
            //print memory starting from base to base+size.
            for (size_t i=base; i<base+size; ++i) {
                cout << "Memory[0x" << hex << i << "] = 0x" << hex << (int)(unsigned char)mem[i] << endl;
            }
        }
        else if (command == "break") {
                if(break_line.size() >=5) {
                    cout << "Maximum Number of Breakpoints reached" << endl;
                    continue;
                }
                int line_number;
                cin >> dec >> line_number;

                break_line.push_back(line_number);

                cout << "Breakpoint Set at line " << line_number << endl;
        }
        else if (command == "del") {
            int line_number;
            string del_cmd;
            cin >> del_cmd;
            cin >> dec >> line_number;

            //To find and delete breakpoint.
            if(break_line.empty()){
                cout << "No breakpoint set at line number " << line_number << endl;               
            }
            else{
                size_t i;
            for (i =0; i < break_line.size(); i++) {
                if(break_line[i] == line_number) {
                    break_line.erase(break_line.begin() + i);
                    cout << "Deleted breakpoint at line " << line_number << endl;
                }
            }
            if(i == break_line.size()) {
                cout << "No breakpoint set at line number " << line_number << endl;
                }
            }
        }
        else if(command == "show-stack") {
            if(call_stack.empty()) {
                cout << "Empty call stack: Execution Complete" << endl;
            }
            else {
                cout << "Call Stack:" << endl;
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