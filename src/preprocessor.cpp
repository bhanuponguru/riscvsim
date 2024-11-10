#include "preprocessor.h"
#include "utils.h"
#include<sstream>

vector<vector<string>> preprocess_and_parse(vector<string> lines, unordered_map<string, unsigned int>& labels, vector<int>& line_numbers, char mem[]) {
    //preprocess the input file, that is ignore all comments and split the tokens.
    vector<string> instructions;
    //find the .data section and store the data in memory.
    bool data_section=false;
    for (auto line=lines.begin(); line != lines.end(); ++line) {
        if (*line == ".data") {
            data_section=true;
            continue;
        }
        if (!data_section) {
            continue;
        }
        if (*line == ".text") {
            break;
        }
        vector<string> tokens;
        string token;
        stringstream ss(*line);
        while (ss >> token) {
            if (token[token.length()-1] == ',') {
                token=token.substr(0, token.length()-1);
            }
            tokens.push_back(token);
        }
        if (tokens.size() == 0) {
            continue;
        }
        if (tokens[0] == ";") {
            continue;
        }
        if (tokens[0] == ".word") {
            for (size_t i=1; i<tokens.size() && tokens[i][0]!=';'; ++i) {
                int val=to_int(tokens[i]);
                //store val in little endian format.
                for (int j=0; j<4; ++j) {
                    mem[j]=val&0xff;
                    val=val>>8;
                }
                mem+=4;
            }
            }
            else if (tokens[0] == ".half") {
            for (size_t i=1; i<tokens.size() && tokens[i][0]!=';'; ++i) {
                int val=to_int(tokens[i]);
                //store val in little endian format.
                for (int j=0; j<2; ++j) {
                    mem[j]=val&0xff;
                    val=val>>8;
                }
                mem+=2;
            }
        }
        else if (tokens[0] == ".dword") {
            for (size_t i=1; i<tokens.size() && tokens[i][0]!=';'; ++i) {
                long long val=to_int(tokens[i]);
                //store val in little endian format.
                for (int j=0; j<8; ++j) {
                    mem[j]=val&0xff;
                    val=val>>8;
                }
                mem+=8;
            }
        }
        else if (tokens[0] == ".byte") {
            for (size_t i=1; i<tokens.size() && tokens[i][0]!=';'; ++i) {
                int val=to_int(tokens[i]);
                *mem=val&0xff;
                ++mem;
            }
        }
    }
    bool text=false;
    //if previously, there is no data section found, then by default text section is true.
    if (!data_section) {
        text=true;
    }
    for (auto line=lines.begin(); line != lines.end(); ++line) {
        if (*line == ".text") {
            text=true;
            continue;
        }
        if (!text) {
            continue;
        }
        string instruction;
        for (auto c=line->begin(); c != line->end(); ++c) {
            if (*c == ';') {
                break;
            }
            instruction.push_back(*c);
        }
        if (instruction[instruction.length()-1] == ' ') {
            instruction=instruction.substr(0, instruction.length()-1);
        }
        if (instruction == "") {
            continue;
        }
        instructions.push_back(instruction);
        line_numbers.push_back(line-lines.begin());
    }
    vector<vector<string>> parsed;
    unsigned int lc=1; //line counter for storing label and it's line number.
    for (auto instruction = instructions.begin(); instruction != instructions.end(); ++instruction) {
        string token;
        vector<string> parsed_instr;
        if (*instruction == "") {
            ++lc;
            parsed.push_back(parsed_instr);
            continue;
        }
        //split the string by space and append each token to the parsed instruction.
        for (size_t i=0; i <= instruction -> length(); ++i) { //we chose <= because we also want to append the last token when we reached end of the instruction. to compare the end, we chose <= instruction->length.
        if ((*instruction)[i] == '(') { //handling the parsing for ld/sd case.
            size_t j=0;
            for (j=i; j<instruction->length() && (*instruction)[j+1] != ')'; ++j);
            parsed_instr.push_back(instruction -> substr(i+1, j-i));
            parsed_instr.push_back(token);
            token="";
            i=j+1;
            continue;
        }
            if ((*instruction)[i] == ' ' || i >= instruction -> length()) {
                if ((*instruction)[i-1] == ':') {
                    if (labels.find(token.substr(0, token.length()-1)) != labels.end()) {
                        cerr << "label " << token.substr(0, token.length()-1) << " already exists." << endl;
                        exit(1);
                    }
                    labels[token.substr(0, token.length()-1)]=lc;
                }
                else if ((*instruction)[i-1] == ',') {
                    parsed_instr.push_back(token.substr(0, token.length()-1));
                }
                else {
                    parsed_instr.push_back(token);
                }
                token="";
                continue;
            }
            token.push_back((*instruction)[i]);
        }
        if (parsed_instr[parsed_instr.size()-1] == "") {
            parsed_instr.pop_back();
        }
        parsed.push_back(parsed_instr);
        ++lc;
    }
    //loop through all parsed and trim empty spaces.
    for (auto instr=parsed.begin(); instr != parsed.end(); ++instr) {
        for (auto token=instr->begin(); token != instr->end(); ++token) {
            *token=trim_space(*token);
        }
    }
    //loop through parsed instructions and delete all empty tokens.
    for (auto instr=parsed.begin(); instr != parsed.end(); ++instr) {
        vector<int> empty_tokens;
        for (auto token=instr->begin(); token != instr->end(); ++token) {
            if (*token == "") {
                empty_tokens.push_back(token-instr->begin());
            }
        }
        for (auto token=empty_tokens.rbegin(); token != empty_tokens.rend(); ++token) {
            instr->erase(instr->begin()+*token);
        }
    }
    return parsed;
}

