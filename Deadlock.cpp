#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h> 
using namespace std;

struct Process {
    int pid;
    string holds;
    string requests;
};

map<int, vector<int>> graph;

bool dfs(int node, map<int,bool> &vis, map<int,bool> &stack) {
    vis[node] = stack[node] = true;

    for(int next : graph[node]) {
        if(!vis[next] && dfs(next, vis, stack)) return true;
        else if(stack[next]) return true;
    }
    stack[node] = false;
    return false;
}

int main() {

    int fd = open("data.txt", O_RDONLY);  
    if(fd < 0) {
        perror("File open failed");
        return 1;
    }

    char buffer[1024];
    int bytes = read(fd, buffer, sizeof(buffer)); 
    close(fd);

    if(bytes <= 0){
        cout << "File is empty or unreadable\n";
        return 0;
    }

    buffer[bytes] = '\0';

    vector<Process> processes;
    stringstream ss(buffer);
    Process p;

    while(ss >> p.pid >> p.holds >> p.requests) {
        processes.push_back(p);
    }

    for(auto &x : processes){
        for(auto &y : processes){
            if(x.pid != y.pid && x.requests == y.holds){
                graph[x.pid].push_back(y.pid);
            }
        }
    }

    map<int,bool> vis, instack;
    bool deadlock = false;

    for(auto &pr : processes){
        if(!vis[pr.pid])
            if(dfs(pr.pid, vis, instack)) {
                deadlock = true; break;
            }
    }

    if(deadlock) cout << "\n🔴 Deadlock Detected! (Cycle Present)\n";
    else         cout << "\n🟢 No Deadlock Found.\n";

    return 0;
}
