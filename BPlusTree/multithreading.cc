#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ctime>
#include "bpt.h"
#include "variables.h"
#include <chrono>

#include <sys/wait.h>
#include <unistd.h>

#define UNDERLINE "\033[4m"
#define CLOSEUNDERLINE "\033[0m"

std::pair<long long, long long> threadResults[100];

void multithread(int left, int right, const int threadNumber);
void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset);
void spawnChild(int i,int a , int  b);
uint64_t timeSinceEpochMillisec();

using namespace bpt;

int main(void){

    bpt::bplus_tree database(DB_NAME);
    meta_t meta = database.get_meta();
    leaf_node_t leaf;
    int number_of_threads = meta.number_of_threads;

    // set the last offset as zero
    meta.thread_offsets[number_of_threads] = 0;


    meta.thread_offsets[number_of_threads] = 0;
    clock_t start, end;


    start = clock();
    printf(UNDERLINE "\nSingle Process\n" CLOSEUNDERLINE);
    multithread_aggregate(0, meta.thread_offsets[0], 0);
    printf("Sum : %lld ,Count : %lld \n\n", threadResults[0].first,threadResults[0].second);
    end = clock();
    // printf("Sum: %d, count: %d\n", ans.first, ans.second);

    double stt = (end - start) / (double)(CLOCKS_PER_SEC);
    printf("time taken by Single Process: %f s\n\n\n\n", (end - start) / (double)(CLOCKS_PER_SEC));



    printf(UNDERLINE "Multiple processes - Number of offsets: %ld\n\n" CLOSEUNDERLINE,meta.number_of_threads );
    for(size_t i=0; i<meta.number_of_threads; ++i){
         database.run_map(&leaf, meta.thread_offsets[i]);
         printf("Process %lu runs from offset: %d\n",i, leaf.children[0].value);
    }
    printf("\n");

    uint64_t startTimeForMultiProc =timeSinceEpochMillisec();
    for (size_t i = 0; i < meta.number_of_threads ; ++i){
        spawnChild(i, meta.thread_offsets[i], meta.thread_offsets[i+1]);
    }

    
    for(size_t i=0; i<meta.number_of_threads; i++){
      wait(NULL);
    }

    long long int fsum =0, fcount = 0;
    for(size_t i  =  0;i<meta.number_of_threads;i++){
        fsum += threadResults[i].first;
        fcount += threadResults[i].second;
    }
    printf("Sum: %lld, count: %lld\n", fsum, fcount);   
    
    uint64_t endTimeForMultiProc = timeSinceEpochMillisec();

    // end = clock();
    double mtt = (endTimeForMultiProc - startTimeForMultiProc)/1000.0;
    printf("time taken by Multiprocessing: %f s\n\n", mtt);

    double percentage = (stt/mtt) ;
    printf("Multiprocessing is %fx faster than a single process\n",percentage);
  
    return 0;

}


void spawnChild(int i , int startOffset , int endOffset){
    pid_t pid = fork();
    if(pid == 0){
        // clock_t start = clock();
        multithread_aggregate(i,startOffset,endOffset);
        // clock_t end = clock();
        // printf("time taken by process %d is %f s\n", i, (end - start) / (double)(CLOCKS_PER_SEC));
        exit(0);
    }
}

uint64_t timeSinceEpochMillisec() {
  using namespace std::chrono;
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset)
{
    long long sum = 0;
    long long c = 0;

    bplus_tree database(DB_NAME);
    leaf_node_t temp;
    database.run_map(&temp, start_leaf_offset);
    while (temp.next != end_leaf_offset){
        for (size_t i = 0; i < temp.n; ++i)
        {
            sum += temp.children[i].value;
            c++;
        }
        database.run_map(&temp, temp.next);
    }
    if(end_leaf_offset==0){
        for (size_t i = 0; i < temp.n; ++i){
            sum += temp.children[i].value;
            c++;
        }
    }


    threadResults[thread_number] = {sum, c};
}

// void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset)
// {
//     printf("multithreaded here\n");
//     long long sum = 0;
//     long long c = 0;

//     bplus_tree database(DB_NAME);
//     leaf_node_t temp;
//     database.run_map(&temp, start_leaf_offset);

//     off_t end_leaf_offset = 0;

//     while (temp.next != end_leaf_offset)
//     {
//         for (int i = 0; i < temp.n; ++i)
//         {
//             sum += temp.children[i].value;
//             c++;
//         }
//         database.run_map(&temp, temp.next);
//     }

//     for (int i = 0; i < temp.n; ++i)
//     {
//         sum += temp.children[i].value;
//         c++;
//     }
//     database.run_map(&temp, temp.next);
//     // printf("sum : %lld\n", sum);

//     threadResults[thread_number] = {sum, c};
// }
