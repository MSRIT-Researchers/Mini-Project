// C program for passing value from
// child process to parent process
#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define MAX 10
using namespace std;

int fd[2];
int arr[] = {0,0,0,0,0};
int spawnChild(int i ){
    pid_t pid = fork();
    if(pid == 0){

        // no need to use the read end of pipe here so close it
        close(fd[0]);

        // closing the standard output
        close(1);	
        arr[i] = 1;
        write(fd[1], arr, sizeof(arr));

        exit(0);
    }
    else{
        return pid;
    }
}
int main(){

    int i = 0;
    pipe(fd);
    cout<<fd[0]<<" "<<fd[1]<<endl;
     spawnChild(0);
     spawnChild(1);


    close(0);

    // no need to use the write end of pipe here so close it
    close(fd[1]);


    // n stores the total bytes read successfully
    // int n = read(fd[0], arr, sizeof(arr));
    for ( i = 0;i < sizeof(arr)/sizeof(int); i++)
        printf("%d ", arr[i]);
   
}
