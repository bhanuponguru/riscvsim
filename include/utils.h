#pragma once
#include<string>
#include<unordered_map>
#include<vector>
using namespace std;

string int_to_hex(long long n, unsigned int count=8, bool sign_extend=false);
int sign_extend(int num, int bits);

int get_label(string s, int x, unordered_map<string, unsigned int> labels);
int get_register(string s, int x);

void load_from_file(string filename, char memory[], long long registers[], int& pc, unordered_map<string, unsigned int>& labels, vector<int>& line_numbers, vector<string>& lines);

int to_int(string num);