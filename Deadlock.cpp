#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

bool isComment(const string &s) {
    for (char c : s)
        if (c == '#') return true;
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

    int fd = open("data.txt", O_RDONLY);
    if (fd < 0) { perror("File open failed"); return 1; }

    char buf[4096];
    int bytes = read(fd, buf, sizeof(buf));
    close(fd);

    if (bytes <= 0) {
        cout << "❌ ERROR: File empty or unreadable.\n";
        return 1;
    }

    buf[bytes] = '\0';
    stringstream ss(buf);
    string line;
    vector<string> tokens;

    while (getline(ss, line)) {
        if (line.size() == 0) continue;
        if (line[0] == '#') continue;
        if (isComment(line)) continue;
        tokens.push_back(line);
    }

    if (tokens.size() < 3) {
        cout << "❌ ERROR: Invalid or incomplete data.\n";
        return 1;
    }

    int idx = 0;
    int n = stoi(tokens[idx++]); 
    int m = stoi(tokens[idx++]);  

    vector<vector<int>> alloc(n, vector<int>(m));
    vector<vector<int>> maxm(n, vector<int>(m));
    vector<int> avail(m);

    for (int i = 0; i < n; i++) {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++)
            row >> alloc[i][j];
    }

    for (int i = 0; i < n; i++) {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++)
            row >> maxm[i][j];
    }

    {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++)
            row >> avail[j];
    }

    unordered_map<int, vector<int>> G;

    for (int p = 0; p < n; p++) {
        for (int r = 0; r < m; r++) {

            if (alloc[p][r] > 0)
                G[100 + r].push_back(p);    

            if (maxm[p][r] - alloc[p][r] > 0)
                G[p].push_back(100 + r);     
        }
    }

    unordered_map<int,bool> vis, instack;
    bool circularWait = false;

    for (auto &x : G)
        if (!vis[x.first] && dfs(x.first, G, vis, instack))
            circularWait = true;

    cout << "\n================ CIRCULAR WAIT CHECK ================\n";
    if (circularWait)
        cout << "🔴 Circular Wait Detected (Cycle in RAG)\n";
    else
        cout << "🟢 No Circular Wait.\n";


    vector<vector<int>> need(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = maxm[i][j] - alloc[i][j];

    vector<bool> done(n, false);
    vector<int> safe;

    vector<int> avail_copy = avail;

    for (int step = 0; step < n; step++) {
        bool found = false;

        for (int p = 0; p < n; p++) {
            if (!done[p]) {
                bool ok = true;
                for (int r = 0; r < m; r++)
                    if (need[p][r] > avail_copy[r])
                        ok = false;

                if (ok) {
                    for (int r = 0; r < m; r++)
                        avail_copy[r] += alloc[p][r];

                    safe.push_back(p);
                    done[p] = true;
                    found = true;
                }
            }
        }

        if (!found) break;
    }

    cout << "\n================ BANKER'S ALGORITHM ================\n";
    if (safe.size() != n) {
        cout << "🔴 Deadlock Detected — No Safe Sequence.\n";
        return 0;
    }

    cout << "🟢 Safe State Found\nSafe Sequence: ";
    for (int p : safe) cout << "P" << p << " ";
    cout << "\n";

    
    cout << "\n================ PROCESS SIMULATION ================\n";

    vector<int> availSim = avail;

    for (int p : safe) {

        cout << "\n▶ Running P" << p << "\n";
        cout << "   Need = [ ";
        for (int j = 0; j < m; j++) cout << need[p][j] << " ";
        cout << "] | Available = [ ";
        for (int j = 0; j < m; j++) cout << availSim[j] << " ";
        cout << "]\n";

        sleep(1);

        cout << "   ✔ P" << p << " completed. Releasing resources.\n";

        for (int j = 0; j < m; j++)
            availSim[j] += alloc[p][j];

        cout << "   New Available = [ ";
        for (int j = 0; j < m; j++) cout << availSim[j] << " ";
        cout << "]\n";
    }

    cout << "\n🎉 All Processes Completed Successfully — SAFE EXECUTION.\n";

    return 0;
}