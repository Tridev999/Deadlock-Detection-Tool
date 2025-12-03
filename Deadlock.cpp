#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;

bool isComment(const string &s) {
    for (char c : s)
        if (c == '#') return true;
    return false;
}

int main() {

    int fd = open("data.txt", O_RDONLY);
    if (fd < 0) {
        perror("File open failed");
        return 1;
    }

    char buffer[4096];
    int bytes = read(fd, buffer, sizeof(buffer));
    close(fd);

    if (bytes <= 0) {
        cout << "❌ ERROR: File empty or unreadable.\n";
        return 1;
    }

    buffer[bytes] = '\0';
    stringstream ss(buffer);

    string line;
    vector<string> tokens;

    while (getline(ss, line)) {
        if (line.size() == 0) continue;
        if (line[0] == '#') continue;
        if (isComment(line)) continue;
        tokens.push_back(line);
    }

    if (tokens.size() < 3) {
        cout << "❌ ERROR: Input too small or missing sections.\n";
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
        for (int j = 0; j < m; j++) {
            if (!(row >> alloc[i][j])) {
                cout << "❌ ERROR: Invalid Allocation row at P" << i << "\n";
                return 1;
            }
        }
    }

    for (int i = 0; i < n; i++) {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++) {
            if (!(row >> maxm[i][j])) {
                cout << "❌ ERROR: Invalid Max row at P" << i << "\n";
                return 1;
            }
        }
    }

    {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++) {
            if (!(row >> avail[j])) {
                cout << "❌ ERROR: Invalid Available vector.\n";
                return 1;
            }
        }
    }


    vector<vector<int>> need(n, vector<int>(m));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            need[i][j] = maxm[i][j] - alloc[i][j];

    vector<int> safeSeq;
    vector<bool> done(n, false);

    for (int k = 0; k < n; k++) {
        bool found = false;

        for (int p = 0; p < n; p++) {
            if (!done[p]) {
                bool canRun = true;
                for (int r = 0; r < m; r++) {
                    if (need[p][r] > avail[r]) {
                        canRun = false;
                        break;
                    }
                }

                if (canRun) {

                    for (int r = 0; r < m; r++)
                        avail[r] += alloc[p][r];

                    safeSeq.push_back(p);
                    done[p] = true;
                    found = true;
                }
            }
        }

        if (!found) break;
    }

    if (safeSeq.size() != n) {
        cout << "\n🔴 DEADLOCK Detected! No Safe Sequence.\n";
        return 0;
    }

    
    cout << "\n🟢 No Deadlock. Safe Sequence Found.\n";
    cout << "➡ Safe Sequence: ";
    for (int x : safeSeq) cout << "P" << x << " ";
    cout << "\n\n📌 Starting Process Simulation...\n\n";

    idx = 2;
    avail.clear();
    avail.resize(m);

    idx = 2 + n + n; 
    {
        stringstream row(tokens[idx++]);
        for (int j = 0; j < m; j++)
            row >> avail[j];
    }

    for (int process : safeSeq) {
        cout << "-------------------------------------------\n";
        cout << "🔹 Process P" << process << " requesting resources:\n";
        cout << "   Need = [ ";
        for (int j = 0; j < m; j++) cout << need[process][j] << " ";
        cout << "]\n";

        cout << "   Available = [ ";
        for (int j = 0; j < m; j++) cout << avail[j] << " ";
        cout << "]\n";

        cout << "\n   ✔ Resources allocated to P" << process << "\n";
        cout << "   ✔ P" << process << " is RUNNING...\n";
        sleep(1);

        cout << "   ✔ P" << process << " COMPLETED.\n";
        cout << "   ✔ Releasing resources back to system.\n";

        for (int j = 0; j < m; j++)
            avail[j] += alloc[process][j];

        cout << "   New Available = [ ";
        for (int j = 0; j < m; j++) cout << avail[j] << " ";
        cout << "]\n\n";
    }

    cout << "-------------------------------------------\n";
    cout << "🎉 All processes finished successfully!\n";
    cout << "🎉 System remained in a SAFE state.\n";

    return 0;
}
