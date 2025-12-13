#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

bool isComment(const string &s) {
    for (char c : s) if (c == '#') return true;
    return false;
}

bool dfs(int u, unordered_map<int, vector<int>> &G,
         unordered_map<int,bool> &vis, unordered_map<int,bool> &instack)
{
    vis[u] = instack[u] = true;
    for (int v : G[u]) {
        if (!vis[v] && dfs(v, G, vis, instack)) return true;
        if (instack[v]) return true;
    }
    instack[u] = false;
    return false;
}

int main() {
    string filename;
    cout << CYAN << "\nEnter input file name: " << RESET;
    cin >> filename;

    int fd = open(filename.c_str(), O_RDONLY);
    if (fd < 0) { perror("File open failed"); return 1; }

    char buf[8192];
    int bytes = read(fd, buf, sizeof(buf));
    close(fd);
    if (bytes <= 0) { cout << RED << "ERROR: File empty or unreadable.\n" << RESET; return 1; }

    buf[bytes] = '\0';

    stringstream ss(buf);
    string line;
    vector<string> tokens;

    while (getline(ss, line)) {
        size_t p = line.find_first_not_of(" \t\r\n");
        if (p == string::npos) continue;
        if (line[p] == '#') continue;
        if (isComment(line)) continue;
        tokens.push_back(line);
    }

    if (tokens.size() < 3) { cout << RED << "ERROR: Invalid or incomplete data.\n" << RESET; return 1; }

    int idx = 0;
    int n = stoi(tokens[idx++]);
    int m = stoi(tokens[idx++]);

    if (n <= 0 || m <= 0) { cout << RED << "ERROR: n or m cannot be zero/negative.\n" << RESET; return 1; }

    vector<vector<int>> alloc(n, vector<int>(m));
    vector<vector<int>> maxm(n, vector<int>(m));
    vector<int> avail(m);

    for (int i = 0; i < n; ++i) {
        if (idx >= tokens.size()) { cout << RED << "ERROR: Missing allocation rows.\n" << RESET; return 1; }
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; ++j) { row >> alloc[i][j]; if (alloc[i][j] < 0) { cout << RED << "ERROR: Negative allocation.\n" << RESET; return 1; } }
    }

    for (int i = 0; i < n; ++i) {
        if (idx >= tokens.size()) { cout << RED << "ERROR: Missing max rows.\n" << RESET; return 1; }
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; ++j) {
            row >> maxm[i][j];
            if (maxm[i][j] < 0) { cout << RED << "ERROR: Negative max.\n" << RESET; return 1; }
            if (alloc[i][j] > maxm[i][j]) { cout << RED << "ERROR: alloc > max.\n" << RESET; return 1; }
        }
    }

    {
        if (idx >= tokens.size()) { cout << RED << "ERROR: Missing available row.\n" << RESET; return 1; }
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; ++j) { row >> avail[j]; if (avail[j] < 0) { cout << RED << "ERROR: Negative available value.\n" << RESET; return 1; } }
    }

    unordered_map<int, vector<int>> G;

    for (int p = 0; p < n; ++p) {
        for (int r = 0; r < m; ++r) {
            if (alloc[p][r] > 0) G[100 + r].push_back(p);
            if (maxm[p][r] - alloc[p][r] > 0) G[p].push_back(100 + r);
        }
    }

    cout << MAGENTA << "\n========= RESOURCE ALLOCATION GRAPH =========\n" << RESET;

    for (int p = 0; p < n; ++p) {
        if (G[p].empty()) continue;
        cout << MAGENTA << "P" << p << " -> ";
        for (int node : G[p]) cout << "R" << (node - 100) << " ";
        cout << RESET << "\n";
    }

    for (int r = 0; r < m; ++r) {
        int rid = 100 + r;
        if (G[rid].empty()) continue;
        cout << MAGENTA << "R" << r << " -> ";
        for (int p : G[rid]) cout << "P" << p << " ";
        cout << RESET << "\n";
    }

    cout << YELLOW << "\n========= CIRCULAR WAIT CHECK =========\n" << RESET;

    unordered_map<int,bool> vis, instack;
    bool circularWait = false;

    for (auto &kv : G) {
        int node = kv.first;
        if (!vis[node] && dfs(node, G, vis, instack)) { circularWait = true; break; }
    }

    if (circularWait) cout << RED << "🔴 Circular Wait Detected\n" << RESET;
    else cout << GREEN << "🟢 No Circular Wait\n" << RESET;

    cout << YELLOW << "\n========= BANKER'S ALGORITHM =========\n" << RESET;

    vector<vector<int>> need(n, vector<int>(m));
    for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) need[i][j] = maxm[i][j] - alloc[i][j];

    vector<int> avail_copy = avail;
    vector<bool> done(n, false);
    vector<int> safe;

    for (int step = 0; step < n; ++step) {
        bool found = false;
        for (int p = 0; p < n; ++p) {
            if (done[p]) continue;
            bool ok = true;
            for (int r = 0; r < m; ++r) if (need[p][r] > avail_copy[r]) ok = false;

            if (ok) {
                for (int r = 0; r < m; ++r) avail_copy[r] += alloc[p][r];
                done[p] = true;
                safe.push_back(p);
                found = true;
            }
        }
        if (!found) break;
    }

    if (safe.size() != n) {
        cout << RED << "🔴 DEADLOCK — No Safe Sequence\n" << RESET;
        return 0;
    }

    cout << GREEN << "🟢 SAFE STATE FOUND\nSafe Sequence: ";
    for (int p : safe) cout << "P" << p << " ";
    cout << RESET << "\n";

    cout << CYAN << "\n========= PROCESS SIMULATION =========\n" << RESET;

    vector<int> availSim = avail;

    for (int p : safe) {
        cout << CYAN << "\n▶ Running P" << p << RESET << "\n";
        cout << " Need = [ "; for (int r = 0; r < m; ++r) cout << need[p][r] << " "; cout << "]";
        cout << "  Available = [ "; for (int r = 0; r < m; ++r) cout << availSim[r] << " "; cout << "]\n";
        sleep(1);
        cout << GREEN << " ✔ P" << p << " completed.\n" << RESET;
        for (int r = 0; r < m; ++r) availSim[r] += alloc[p][r];
        cout << " New Available = [ "; for (int r = 0; r < m; ++r) cout << availSim[r] << " "; cout << "]\n";
    }

    cout << GREEN << "\n All Processes Finished — SAFE EXECUTION.\n" << RESET;
    return 0;
}

