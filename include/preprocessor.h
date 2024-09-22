#pragma once
#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>

using namespace std;

vector<vector<string>> preprocess_and_parse(vector<string> instructions, unordered_map<string, unsigned int>& labels, vector<int>& line_numbers);
