#ifndef MISC_FUNC
#define MISC_FUNC 

#include <iostream>
#include <map>
#include <string>
#include <cassert>

using namespace std;
using namespace global;

// map<instance line number, machine id/line number>
vector<pair<int, int> > process_output(const map<int, int>& output);

// map<instance id, machine id>
void write_output(const vector<pair<int, int> >& output, string file_name);

void write_output_turn(const vector<vector<pair<int, int> > >& output, string file_name); 

void write_output_origin(const map<int, int>& output, string file_name) ;

// map<instance line number, machine id/line number>
void check_output(const map<int, int>& output);

void check_output_file(string output_file_name) ;

vector<pair<int, int> > process_output2(map<int, int>& output) ;

void transform_pos( map<int, int> &pos , vector<int> & ins_mch ); 

void reverse_pos( map<int, int> &pos , vector<int> & ins_mch );

void write_offline_result(string file_name , vector<tuple<int,int,int> > &result);

#endif
