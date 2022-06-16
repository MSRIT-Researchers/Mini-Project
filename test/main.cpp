#include <bits/stdc++.h>
#include <future>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
vector<int> v;
long long calc(int l , int r){
    long long sum = 0;
    for(int i = l; i <= r; i++){
        sum += v[i];
    }

    return sum;
}
pid_t spawnChild(int l, int r){
    pid_t pid = fork();
    if(pid == 0){
        long long sum = calc(l, r);
        // cout<<sum<<endl;
        exit(0);
    }
    return pid;
}

int main(){
    clock_t start, end;
    int n = 40000000;
    for(int i = 0 ;i<n;i++)v.push_back(1);
    start = clock();
    long long sum = 0;
    for(int i = 0; i < n; i++){
        sum += v[i];
    }

    end = clock();
    printf("time taken %f s\n",  (end - start) / (double)(CLOCKS_PER_SEC));

    start = clock();
    int l =0 , r = n/10-1;
    vector<long long> children(n/10),sumar(n/10);
    int i = 0;
    while(r<n){
        spawnChild(l,r);        
        i++;
        l= r+1;
        r+=n/10;
    }


    pid_t child_pid;

    while ((child_pid = wait(nullptr)) > 0);
    end = clock();
    printf("time taken %f s\n",  (end - start) / (double)(CLOCKS_PER_SEC));

}
