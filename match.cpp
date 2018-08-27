#include "match.h"

//----variables global
vector<vector<int> > O;
vector<int> output;
int edge_num;

void KM_Match::reset(int input_num, int _cut, double edge[MAX_N][MAX_N])
{
    nx = ny = input_num;
    cut = _cut; 
    for (int i=0;i<=nx;i++) g[i] =edge[i];
}

vector<int> KM_Match::KM()
{
    score = 0;
    for (int i=1;i<=nx;i++) {
        girl[i] = i;
        score += g[i][i];
    }
    int max_times = 100000000;
    
    for (int times=0;times<max_times;times++) {
        int tmp_a = rand()%nx+1, tmp_b = rand()%nx+1;
        if (tmp_a == tmp_b || ((tmp_a>cut)!=(tmp_b>cut))) continue;
        if (g[tmp_a][girl[tmp_a]] + g[tmp_b][girl[tmp_b]]
            > g[tmp_a][girl[tmp_b]] + g[tmp_b][girl[tmp_a]])
        {
            score += g[tmp_a][girl[tmp_b]] + g[tmp_b][girl[tmp_a]]
                        - g[tmp_a][girl[tmp_a]] - g[tmp_b][girl[tmp_b]];
            swap(girl[tmp_a],girl[tmp_b]);
        }
        if (times %1000000==0) cout << times << " " << score << endl;
    }
    vector<int> results = {0};
    for (int i=1;i<=nx;i++) results.push_back(girl[i]);
    return results;
}

