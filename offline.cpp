#include "offline.h"

using namespace global;
using namespace std;
const int _TIME_LEN = 98;
const int _TIME_DUR = 15;
const int _MAX_TIME_LEN = _TIME_LEN * _TIME_DUR + 5;
const int _MAX_MACHINE_NUM = 1e4;
const int _MAX_JOB = 1e4;
const double _EPS = 1e-8;

#define CHL (o << 1) 
#define CHR ((o << 1) | 1)
#define MID ((r + l) >> 1)

template<typename T, int range, bool bf=false>
struct RMQ {
    vector<T> value, add, max_v;
    RMQ() {
        value = vector<T>(range*4, 0);
        add = vector<T>(range*4, 0);
        max_v = vector<T>(range*4, 0);
    }
    void range_add_t(int a, int b, T x) {
        for(int i = a; i < b; i++)
            value[i] += x;
    }
    T range_max_t(int a, int b) {
        T max_value = 0;
        for(int i = a; i < b; i++) 
            max_value = max(max_value, value[i]);
        return max_value;
    }

    void push_up(int o, int l, int r) {
        if(r - l <= 1) return;
        max_v[o] = max(max_v[CHL], max_v[CHR]);
    }
    void push_down(int o, int l, int r) {
        if(r - l <= 1) return;
        max_v[CHL] += add[o];
        max_v[CHR] += add[o];
        add[CHL] += add[o];
        add[CHR] += add[o];
        add[o] = 0;
    }
    void _range_add_seg(int o, int l, int r, int a, int b, T x) {
        if(l >= b || r <= a) return;
        if(a <= l && b >= r) {
            max_v[o] += x;
            add[o] += x;
            return;
        }
        push_down(o, l, r);
        _range_add_seg(CHL, l, MID, a, b, x);
        _range_add_seg(CHR, MID, r, a, b, x);
        push_up(o, l, r);
    }
    void range_add_seg(int a, int b, T x) {
        _range_add_seg(1, 0, range, a, b, x);
    }

    T _range_max_seg(int o, int l, int r, int a, int b) {
        push_down(o, l, r);
        //cerr << o << " " << l << " " << r << " " << a << " " << b << endl;
        if(l >= b || r <= a) return 0;
        if(a <= l && b >= r) return max_v[o];
        T res = max(_range_max_seg(CHL, l, MID, a, b),
                    _range_max_seg(CHR, MID, r, a, b));
        return res;
    }
    T range_max_seg(int a, int b) {
        return _range_max_seg(1, 0, range, a, b);
    }

    void range_add(int a, int b, T c) {
        assert(a >= 0 && b <= _MAX_TIME_LEN);
        if(bf) range_add_t(a, b, c);
        else range_add_seg(a, b, c);
    }

    T range_max(int a, int b) {
        if(bf) return range_max_t(a, b);
        else return range_max_seg(a, b);
    }
};

struct OrderGenerator {
    int reverse_cnt[_MAX_JOB];
    int max_len[_MAX_JOB], max_link[_MAX_JOB];
    map<int, vector<int> > succeeds;
    priority_queue<pair<int, int> > vq;
    OrderGenerator() {
        memset(max_len, 0, sizeof max_len);
        memset(reverse_cnt, 0, sizeof reverse_cnt);
        memset(max_link, 0, sizeof max_link);
        for(Jobs& job: job_res) {
            for(int i = 0; i < job.fn; i++) {
                reverse_cnt[job.f[i]] += 1;
                succeeds[job.f[i]].push_back(i);
            }
        }
        queue<int> Q;
        for(int i = 0; i < (int)job_res.size(); i++) {
            if(reverse_cnt[i] == 0) {
                Q.push(i);
            }
        }
        while(!Q.empty()) {
            int v = Q.front();
            Q.pop();
            max_len[v] = max(max_len[v], job_res[v].time);
            for(int i = 0; i < job_res[v].fn; i++) {
                int pre = job_res[v].f[i];
                if(max_len[pre] < max_len[v] + job_res[pre].time) {
                    assert(job_res[v].time > 0);
                    max_len[pre] = max_len[v] + job_res[pre].time;
                    max_link[pre] = v;
                }
                reverse_cnt[pre] -= 1;
                if(reverse_cnt[pre] == 0) {
                    Q.push(pre);
                }
            }
        }
        for(int i = 0; i < (int)job_res.size(); i++) {
            vq.push(make_pair(max_len[i], i));
        }
    }
    int next() {
        if(vq.empty()) return -1;
        pair<int, int> len_v = vq.top();
        vq.pop();
        return len_v.second;
    }
};

int _last_time[_MAX_JOB];

int first_valid_pos(int job_id) {
    int max_v = 0;
    for(int i = 0; i < job_res[job_id].fn; i++) {
        max_v = max(max_v, _last_time[job_res[job_id].f[i]]);
    }
    return max_v;
}

RMQ<double, _MAX_TIME_LEN, true> _rmq[_MAX_MACHINE_NUM][2];
vector<int> _empty_machines, _other_machines;
int _machine_cnt[_MAX_MACHINE_NUM], _is_empty[_MAX_MACHINE_NUM];

int first_empty_pos(
    int start_pos,
    int job_id,
    int machine_id) {
    int pos = -1;
    double lim = 0.5;
    if(_is_empty[machine_id]) lim = 1;
    for(int j = start_pos; j + job_res[job_id].time <= 1470; j++) {
        double rm_cpu = _rmq[machine_id][0].range_max(j, j + job_res[job_id].time);
        double rm_mem = _rmq[machine_id][1].range_max(j, j + job_res[job_id].time);
        ///cerr << rm_cpu << " " << rm_mem << endl;
        if(rm_cpu + job_res[job_id].cpu <= cpu_spec[machine_id]*lim + _EPS 
            && rm_mem + job_res[job_id].mem <= mem_spec[machine_id] + _EPS) {
            pos = j;
            break;
        }
    }
    if(pos != -1) {
        double cpu_max = _rmq[machine_id][0].range_max(pos, pos + job_res[job_id].time);
        double mem_max = _rmq[machine_id][1].range_max(pos, pos + job_res[job_id].time);
        //cerr << cpu_max + job_res[job_id].cpu << " " << cpu_spec[machine_id] + _EPS << endl;
        //cerr << mem_max + job_res[job_id].mem << " " << mem_spec[machine_id] + _EPS << endl;
        assert(cpu_max + job_res[job_id].cpu <= cpu_spec[machine_id] + _EPS);
        assert(mem_max + job_res[job_id].mem <= mem_spec[machine_id] + _EPS);
    }
    return pos;
}

priority_queue<pair<int, int> > first_empty_pos(
    int start_pos,
    int job_id) {
    priority_queue<pair<int, int> > resq;
    for(int i: _empty_machines) {
        int pos = first_empty_pos(start_pos, job_id, i);
        if(pos != -1) {
            resq.push(make_pair(-pos, i));
        }
    }
    for(int i: _other_machines) {
        int pos = first_empty_pos(start_pos, job_id, i);
        if(pos != -1) {
            resq.push(make_pair(-pos, i));
        }
    }
    return resq;
}

bool _machine_id_cmp(int a, int b) {
    if(cpu_spec[a] != cpu_spec[b]) 
        return cpu_spec[a] > cpu_spec[b];
    return mem_spec[a] > mem_spec[b];
}

vector<tuple<int, int, int> > _job_pos_and_time;

bool check_job_pos_and_time(
    const vector<tuple<int, int, int> >& job_pos_and_time,
    const map<int, int> &ins_pos) {
    vector<int> last_time(_MAX_JOB, 0);
    vector<vector<double> > cpu_usage(_MAX_MACHINE_NUM, vector<double>(_MAX_TIME_LEN, 0));
    vector<vector<double> > mem_usage(_MAX_MACHINE_NUM, vector<double>(_MAX_TIME_LEN, 0));
    for(auto it: ins_pos) {
        int app = instance_apps[it.first];
        int mach = it.second;
        for(int i = 0; i < _TIME_LEN; i++) {
            for(int j = 0; j < _TIME_DUR; j++) {
                cpu_usage[mach][i*_TIME_DUR + j] += app_cpu_line[app][i];
                mem_usage[mach][i*_TIME_DUR + j] += app_mem_line[app][i];
                assert(cpu_usage[mach][i*_TIME_DUR + j] <= cpu_spec[mach] + _EPS);
                assert(mem_usage[mach][i*_TIME_DUR + j] <= mem_spec[mach] + _EPS);
            }
        }
    }
    for(const tuple<int, int, int>& tp: job_pos_and_time) {
        int job_id = get<0>(tp);
        int mach_id = get<1>(tp);
        int pos = get<2>(tp);
        last_time[job_id] = max(last_time[job_id], pos + job_res[job_id].time);
    }
    bool relation_check = true;
    for(const tuple<int, int, int>& tp: job_pos_and_time) {
        int job_id = get<0>(tp);
        int mach_id = get<1>(tp);
        int pos = get<2>(tp);
        for(int i = 0; i < job_res[job_id].fn; i++) {
            int pre = job_res[job_id].f[i];
            //cerr << last_time[pre] << " " << pos << endl;
            if(last_time[pre] > pos) {
                relation_check = false;
            }
        }
    }
    if(!relation_check) {
        cerr << "relation_check failed" << endl;
    }
    bool usage_check = true;
    for(const tuple<int, int, int>& tp: job_pos_and_time) {
        int job_id = get<0>(tp);
        int mach_id = get<1>(tp);
        int pos = get<2>(tp);
        for(int i = pos; i < pos + job_res[job_id].time; i++) {
            cpu_usage[mach_id][i] += job_res[job_id].cpu;
            mem_usage[mach_id][i] += job_res[job_id].mem;
            if(cpu_usage[mach_id][i] >= cpu_spec[mach_id] + _EPS ||
                mem_usage[mach_id][i] >= mem_spec[mach_id] + _EPS) {
                usage_check = false;
            }
        }
    }
    if(!usage_check) {
        cerr << "usage_check failed" << endl;
    }
    if(relation_check && usage_check) {
        cerr << "offline check passed" << endl;
    }
    else {
        cerr << "offline check failed" << endl;
        exit(0);
    }
    return usage_check && relation_check;
}

void offline_scheduling(
    map<int, int> &ins_pos, 
    vector<tuple<int, int, int> > &job_pos_and_time,
    int num) {

    // test seg tree
    /*
    RMQ<double, 1000, true> bf;
    RMQ<double, 1000, false> seg;
    for(int i = 0; i < 10000; i++) {
        cerr << i << endl;
        int a = rand() % 1000, b = rand() % 1000;
        if(a > b) swap(a, b);
        double x = rand() % 1000;
        cerr << a << " " << b << " " << x << endl;
        bf.range_add(a, b, x);
        seg.range_add(a, b, x);
        double bfa = bf.range_max(a, b);
        double sega = seg.range_max(a, b);
        cerr << bfa << " " << sega << endl;

        assert(abs(bfa - sega) < _EPS);
    }
    exit(0);
    */


    

    memset(_is_empty, 0, sizeof _is_empty);
    for(auto it: ins_pos) {
        int app = instance_apps[it.first];
        int mach = it.second;
        _machine_cnt[mach] += 1;
        for(int i = 0; i < _TIME_LEN; i++) {
            //cerr << mach << " " << i*_TIME_DUR << " " << i*_TIME_DUR + _TIME_DUR << " " 
                 //<< app << " " << i << endl;
            _rmq[mach][0].range_add(i*_TIME_DUR, i*_TIME_DUR + _TIME_DUR, app_cpu_line[app][i]);
            _rmq[mach][1].range_add(i*_TIME_DUR, i*_TIME_DUR + _TIME_DUR, app_mem_line[app][i]);
        }
    }
    for(int i = 1; i <= machine_resources_num; i++) {
        if(_machine_cnt[i] == 0) {
            _empty_machines.push_back(i);
            _is_empty[i] = 1;
        }
        else {
            _other_machines.push_back(i);
        }
    }
    sort(_empty_machines.begin(), _empty_machines.end(), _machine_id_cmp);
    while((int)_empty_machines.size() > num) {
        _empty_machines.pop_back();
    }

    OrderGenerator od;
    int cc = 0;
    for(int v = od.next(); v != -1; v = od.next()) {
        int start_pos = first_valid_pos(v);
        priority_queue<pair<int, int> > q = first_empty_pos(start_pos, v);
        for(int k = 0; k < job_res[v].ins_size; k++) {
            if(q.empty()) {
                cerr << "no next_p found" << endl;
                exit(0);
            }
            int mach = q.top().second;
            int p = -q.top().first;
            q.pop();
            _job_pos_and_time.push_back(make_tuple(v, mach, p));
            double cpu_max = _rmq[mach][0].range_max(p, p + job_res[v].time);
            double mem_max = _rmq[mach][1].range_max(p, p + job_res[v].time);
            assert(cpu_max + job_res[v].cpu <= cpu_spec[mach] + _EPS);
            assert(mem_max + job_res[v].mem <= mem_spec[mach] + _EPS);
            //cerr << "ss" << endl;   
            _rmq[mach][0].range_add(p, p + job_res[v].time, job_res[v].cpu);
            _rmq[mach][1].range_add(p, p + job_res[v].time, job_res[v].mem);
            /*
            double cpu_max = _rmq[mach][0].range_max(p, p + job_res[v].time);
            double mem_max = _rmq[mach][1].range_max(p, p + job_res[v].time);
            assert(cpu_max <= cpu_spec[mach] + _EPS);
            assert(mem_max <= mem_spec[mach] + _EPS);
            */
            _last_time[v] = max(_last_time[v], p + job_res[v].time);
            //cerr << p << " " << job_res[v].time << endl;
            int next_p = first_empty_pos(p, v, mach);
            if(next_p != -1) {
                q.push(make_pair(-next_p, mach));
                //cerr << next_p << endl;
            }
        }
        cerr << (++cc) << " v: " << v << " " << _last_time[v] << endl;
        if(cc >= 1000) {
            //break;
        }
    }
    job_pos_and_time = _job_pos_and_time;
    check_job_pos_and_time(job_pos_and_time, ins_pos);
}



/*

double _cpu_usage[_MAX_MACHINE_NUM][_MAX_TIME_LEN];
double _mem_usage[_MAX_MACHINE_NUM][_MAX_TIME_LEN];
int _is_empty[_MAX_MACHINE_NUM];
int _puted[_MAX_JOB];
int _max_len[_MAX_JOB];
int _max_link[_MAX_JOB];

map<int, int> _machine_cnt;
vector<tuple<int, int, int> > _job_pos_and_time;

int _cnt[_MAX_JOB];
int _reverse_cnt[_MAX_JOB];
map<int, vector<int> > _succeeds;

vector<int> _topo_order() {
    vector<int> order;
    queue<int> Q;
    for(int i = 0; i < job_res.size(); i++) {
        if(_cnt[i] == 0) {
            Q.push(i);
        }
    }
    while(!Q.empty()) {
        int v = Q.front();
        Q.pop();
        order.push_back(v);
        for(auto u: _succeeds[v]) {
            _cnt[u] -= 1;
            if(_cnt[u] == 0) {
                Q.push(u);
            }
        }
    }
    return order;
}

bool _put_sequence(
    const vector<int>& sequence, 
    int machine_id, 
    int pos, 
    int time_len, 
    double lim) {
    assert(pos + time_len <= 1470);
    int t = pos;
    bool flag = true;
    int j = 0, i = 0;
    int _init_size = _job_pos_and_time.size();
    for(j = 0; j < (int) sequence.size() && flag; j++) {
        for(i = 0; i < job_res[sequence[j]].time && flag; i++) {
            _cpu_usage[machine_id][t] += job_res[sequence[j]].cpu;
            _mem_usage[machine_id][t] += job_res[sequence[j]].mem;
            _job_pos_and_time.push_back(make_tuple(sequence[j], machine_id, t));
            if(_cpu_usage[machine_id][t] > cpu_spec[machine_id]*lim + _EPS ||
               _mem_usage[machine_id][t] > mem_spec[machine_id] + _EPS) {
                flag = false;
                while(i >= 0) {
                    _cpu_usage[machine_id][t] -= job_res[sequence[j]].cpu;
                    _mem_usage[machine_id][t] -= job_res[sequence[j]].mem;
                    i -= 1;
                }
            }
        }
        t += job_res[sequence[j]].time;
    }
    if(flag) return true;
    while(j >= 0) {
        for(i = 0; i < job_res[sequence[j]].time && flag; i++) {
            _cpu_usage[machine_id][t] -= job_res[sequence[j]].cpu;
            _mem_usage[machine_id][t] -= job_res[sequence[j]].mem;
        }
    }
    while(_job_pos_and_time.size() > _init_size) _job_pos_and_time.pop_back();
    return false;
}

bool _put_sequence(
    const vector<int>& sequence,
    int machine_id,
    double lim) {
    int time_len = 0;
    for(int id: sequence) {
        time_len += job_res[id].time;
    }
    assert(time_len <= 1470);
    for(int t = 0; t < 1470; t++) {
        if(t + time_len > 1470) break;
        if(_put_sequence(sequence, machine_id, t, time_len, lim)) {
            return true;
        }
    }
    return false;
}

vector<int> _find_longest_sequence() {
    vector<int> res;
    memset(_reverse_cnt, 0, sizeof _reverse_cnt);
    memset(_max_len, 0, sizeof _max_len);
    memset(_max_link, -1, sizeof _max_link);
    for(Jobs& job: job_res) {
        assert(_puted[job.id] <= job.ins_size);
        if(_puted[job.id] == job.ins_size) continue;
        for(int i = 0; i < job.fn; i++) {
            _reverse_cnt[job.f[i]] += 1;
        }
    }
    queue<int> Q;
    for(int i = 0; i < (int) job_res.size(); i++) {
        if(_reverse_cnt[i] == 0 && _puted[i] < job_res[i].ins_size) {
            res.push_back(i);
            Q.push(i);
        }
    }
    while(!Q.empty()) {
        int v = Q.front();
        Q.pop();
        if(_max_len[v] == 0) _max_len[v] = job_res[v].time;
        for(int i = 0; i < job_res[v].fn; i++) {
            int pre = job_res[v].f[i];
            _reverse_cnt[pre] -= 1;
            if(_max_len[pre] < _max_len[v] + 1) {
                _max_len[pre] = _max_len[v] + job_res[pre].time;
                _max_link[pre] = v;
            }
            if(_reverse_cnt[pre] == 0 && _puted[pre] < job_res[pre].ins_size) {
                Q.push(pre);
            }
        }
    }
    int __max_len = 0;
    int _max_v = 0;
    for(int i = 0; i < (int) job_res.size(); i++) {
        if(_max_len[i] > __max_len) {
            __max_len = _max_len[i];
            _max_v = i;
        }
    }
    if(__max_len != 0) {
        int v = _max_v;
        while(v != -1) {
            res.push_back(v);
            v = _max_link[v];
        }
    }
    return res;
}

vector<tuple<int, int, int> > feasible_solution(
        const map<int, int>& ins_pos,
        const vector<int>& empty_machines,
        const vector<int>& other_machines) {
    // vector<int> od = _topo_order();
    for(;;) {
        vector<int> seq = _find_longest_sequence();
        if(!seq.size()) break;
        bool flag = false;
        for(int mach_id: empty_machines) {
            if(_put_sequence(seq, mach_id, 1)) {
                flag = true;
                break;
            }
        }
        if(flag) continue;
        for(int mach_id: other_machines) {
            if(_put_sequence(seq, mach_id, 0.5)) {
                flag = true;
                break;
            }
        }
        assert(flag);
    }
    return _job_pos_and_time;
}


bool _machine_id_cmp(int a, int b) {
    if(cpu_spec[a] != cpu_spec[b]) 
        return cpu_spec[a] > cpu_spec[b];
    return mem_spec[a] > mem_spec[b];
}
void offline_scheduling ( 
    map<int, int> &ins_pos, 
    vector<tuple<int, int, int> > &job_pos_and_time,
    int num) {
    memset(_is_empty, 0, sizeof _is_empty);
    memset(_cpu_usage, 0, sizeof _cpu_usage);
    memset(_mem_usage, 0, sizeof _mem_usage);
    memset(_puted, 0, sizeof _puted);
    memset(_reverse_cnt, 0, sizeof _reverse_cnt);
    memset(_cnt, 0, sizeof _cnt);
    for(Jobs& job: job_res) {
        _cnt[job.id] += job.fn;
        for(int i = 0; i < job.fn; i++) {
            _succeeds[job.f[i]].push_back(job.id);
            _reverse_cnt[job.f[i]] += 1;
        }
    }
    map<int, pair<int, int> > res;
    for(auto it: ins_pos) {
        int app = it.first;
        int mach = it.second;
        _machine_cnt[mach] += 1;
        for(int i = 0; i < _TIME_LEN; i++) {
            for(int j = 0; j < _TIME_DUR; j++) {
                _cpu_usage[mach][i*_TIME_DUR + j] += app_cpu_line[app][i];
                _mem_usage[mach][i*_TIME_DUR + j] += app_mem_line[app][i];
            }
        }
    }
    vector<int> empty_machines;
    vector<int> other_machines;
    for(int i = 1; i <= machine_resources_num; i++) {
        if(_machine_cnt[i] == 0) {
            empty_machines.push_back(i);
            _is_empty[i] = 1;
        }
        else {
            other_machines.push_back(i);
        }
    }
    sort(empty_machines.begin(), empty_machines.end(), _machine_id_cmp);
    while((int)empty_machines.size() > num) {
        empty_machines.pop_back();
    }
    job_pos_and_time = feasible_solution(ins_pos, empty_machines, other_machines);
}

*/