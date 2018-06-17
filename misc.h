#include <iostream>
#include <map>
#include <string>
#include <cassert>

using namespace std;
using namespace global;

// map<instance line number, machine id/line number>
map<int, int> process_output(const map<int, int>& output);

// map<instance id, machine id>
void write_output(const map<int, int>& output, string file_name);

// map<instance line number, machine id/line number>
void check_output(const map<int, int>& output);