#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "bpt.h"
#include "variables.h"
#include <future>
#include <sys/wait.h>
#include <unistd.h>

std::pair<long long, long long> threadResults[100];

void multithread(int left, int right, const int threadNumber);
void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset);

using namespace bpt;

void spawnChild(int i , int a , int b){
    pid_t pid = fork();
    if(pid == 0){
        clock_t start = clock();
        multithread_aggregate(i,a,b);
        clock_t end = clock();
        printf("time taken %f s\n",  (end - start) / (double)(CLOCKS_PER_SEC));
        exit(0);
    }
}
int main(void)
{

    bpt::bplus_tree database(DB_NAME);
    int i;
    meta_t meta = database.get_meta();
    int number_of_threads = meta.number_of_threads;
    std::thread threads[meta.number_of_threads];

    leaf_node_t leaf;

    meta.thread_offsets[number_of_threads] = 0;
    printf("Number of Threads: %ld \n\n",meta.number_of_threads );
    for(int i=0; i<meta.number_of_threads; ++i){
         database.run_map(&leaf, meta.thread_offsets[i]);
         printf("Thread %d offset : %d\n",i, leaf.children[0].value);
    }   
    printf("\n");

    clock_t start, end;
    printf("starting time\n");

    start = clock();

    multithread_aggregate(0, meta.thread_offsets[0], 0);
    end = clock();
    // printf("Sum: %d, count: %d\n", ans.first, ans.second);

    double stt = (end - start) / (double)(CLOCKS_PER_SEC);
    printf("time taken by SingleThread: %f s\n", (end - start) / (double)(CLOCKS_PER_SEC));


    std::vector<std::future<void>> futures;
    printf("starting time\n");

    start = clock();
    for (i = 0; i < meta.number_of_threads ; ++i){
        spawnChild(i, meta.thread_offsets[i], meta.thread_offsets[i+1]);

    }

    pid_t child_pid;

    while ((child_pid = wait(nullptr)) > 0){
        // printf("child %d terminated\n",child_pid);
    }

    end = clock();
    printf("\ntime taken by Multithread: %f s\n", (end - start) / (double)(CLOCKS_PER_SEC));
    double mtt = (end - start) / (double)(CLOCKS_PER_SEC);

    double percentage = (stt/mtt) ;
    printf("\nMultithreading is %fx faster than Single threading\n",percentage);
  
    return 0;

}



void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset)
{
    long long sum = 0;
    long long c = 0;

    bplus_tree database(DB_NAME);
    leaf_node_t temp;
    database.run_map(&temp, start_leaf_offset);
    while (temp.next != end_leaf_offset)
    {
        for (int i = 0; i < temp.n; ++i)
        {
            sum += temp.children[i].value;
            c++;
        }
        database.run_map(&temp, temp.next);
    }
// 
    // printf("sum : %lld\n", sum);
    threadResults[thread_number] = {sum, c};
}

void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset)
{
    printf("multithreaded here\n");
    long long sum = 0;
    long long c = 0;

    bplus_tree database(DB_NAME);
    leaf_node_t temp;
    database.run_map(&temp, start_leaf_offset);

    off_t end_leaf_offset = 0;

    while (temp.next != end_leaf_offset)
    {
        for (int i = 0; i < temp.n; ++i)
        {
            sum += temp.children[i].value;
            c++;
        }
        database.run_map(&temp, temp.next);
    }

    for (int i = 0; i < temp.n; ++i)
    {
        sum += temp.children[i].value;
        c++;
    }
    database.run_map(&temp, temp.next);
    // printf("sum : %lld\n", sum);

    threadResults[thread_number] = {sum, c};
}
