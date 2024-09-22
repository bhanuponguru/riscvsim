#include<iostream>
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
    //initialize stack pointer register.
    regs[2]=0x50000;
    vector<string> call_stack; //we are using vector because we have push pop functionality and we can access all the elements.
     initialize_registers();
     while (true) { //an infinite loop to take commands from the user.
        string command;
        cin >> command;
        if (command == "load") {
            string filename;
            cin >> filename;
            load_from_file(filename, mem, regs, pc);
        }
        else if (command == "run") {
            while (pc < 0x50000 && *(int*)(mem+pc) != 0) {
                int instr=*(int*)(mem+pc);
                execute(instr, regs, mem, pc);
                cout << "executed instruction under pc: " << pc << " in line " << pc/4 << endl;
            }
        }
        else if (command == "step") {
            int instr=*(int*)(mem+pc);
            execute(instr, regs, mem, pc);
            cout << "executed instruction under pc: " << pc << " in line " << pc/4 << endl;
        }
        else if (command == "exit") {
            break;
        }
     }
}