#pragma once
#include<string>
#include<unordered_map>
using namespace std;

string int_to_hex(int n);

int get_label(string s, int x, unordered_map<string, unsigned int> labels);
int get_register(string s, int x);

void load_from_file(string filename, char memory[], long long registers[], int& pc);