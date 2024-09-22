#include "preprocessor.h"

vector<vector<string>> preprocess_and_parse(vector<string> lines, unordered_map<string, unsigned int>& labels, vector<int>& line_numbers) {
    //preprocess the input file, that is ignore all comments and split the tokens.
    vector<string> instructions;
    for (auto line=lines.begin(); line != lines.end(); ++line) {
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
    return parsed;
}

