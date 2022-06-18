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
#include <queue>
#include <sys/wait.h>
#include <unistd.h>
#include "multithreading.h"

#define UNDERLINE "\033[4m"
#define CLOSEUNDERLINE "\033[0m"



// using namespace bpt;

    MultiThreadingBPT::MultiThreadingBPT(){

        bpt::bplus_tree database(DB_NAME); 
        bpt::meta_t meta = database.get_meta();

        int number_of_threads = meta.number_of_threads;
        // set the last offset as zero
        meta.thread_offsets[number_of_threads] = 0;

        for(int i=0; i<meta.number_of_threads; ++i){
            this->thread_starts[i] = meta.thread_offsets[i];
        }

        // double singleProcessTime = computeUsingSingleProcess();
        double multiProcessesTime = computeUsingMultipleProcesses();

        // double percentage = (singleProcessTime/multiProcessesTime) ;
        // printf("Multiprocessing is %fx faster than a single process\n",percentage);
    }


    void MultiThreadingBPT::spawnChild(int i , int startOffset , int endOffset){
        pid_t pid = fork();
        if(pid == 0){
            // clock_t start = clock();
            multithread_aggregate(i,startOffset,endOffset);
            // clock_t end = clock();
            // printf("time taken by process %d is %f s\n", i, (end - start) / (double)(CLOCKS_PER_SEC));
            exit(0);
        }
    }

    // Get the time since epoch in milliseconds
    uint64_t MultiThreadingBPT::timeSinceEpochMillisec() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

    double MultiThreadingBPT::computeUsingMultipleProcesses(){

        bpt::bplus_tree database(DB_NAME);
        bpt::meta_t meta = database.get_meta();
        bpt::leaf_node_t leaf;

        printf(UNDERLINE "Multiple processes - Number of offsets: %ld\n\n" CLOSEUNDERLINE,meta.number_of_threads );
        for(size_t i=0; i<meta.number_of_threads; ++i){
            database.run_map(&leaf, meta.thread_offsets[i]);
            printf("Process %lu runs from offset: %d and startnig at %d\n",i, leaf.children[0].value,thread_starts[i]);
        }
        printf("\n");

        uint64_t startTime =timeSinceEpochMillisec();

        
        // while(true){
        //     bool flag = true;
        //     for (size_t i = 0; i < meta.number_of_threads ; ++i){
        //         if(thread_starts[i]<=meta.thread_offsets[i+1]){
        //             printf("spawingng Thread: %d start: %d, end: %d\n", i,thread_starts[i], meta.thread_offsets[i+1]);
        //             spawnChild(i, thread_starts[i],meta.thread_offsets[i+1]);
        //             flag= false;
        //         }
        //     }

        //     // Wait for all processes to complete
        //     for(size_t i=0; i<meta.number_of_threads; i++){
        //         wait(NULL);
        //     }

        //     if(flag)
        //         break;
        // }
        /// aggregate the results
        long long int fsum =0, fcount = 0;
        for(size_t i  =  0;i<meta.number_of_threads;i++){
            fsum += threadResults[i].first;
            fcount += threadResults[i].second;
        }

        printf("Sum: %lld, count: %lld\n", fsum, fcount);   
        
        uint64_t endTime = timeSinceEpochMillisec();

        double multiProcessesTime = (endTime - startTime)/1000.0;
        printf("time taken by Multiprocessing: %f s\n\n", multiProcessesTime);

        return multiProcessesTime;
    }
    double MultiThreadingBPT::computeUsingSingleProcess(){

        clock_t start, end;
        bpt::bplus_tree database(DB_NAME);
        bpt::meta_t meta = database.get_meta();

        start = MultiThreadingBPT::timeSinceEpochMillisec();
        
        printf(UNDERLINE "\nSingle Process\n\n" CLOSEUNDERLINE);
        multithread_aggregate(0, meta.thread_offsets[0], 0);
        printf("Sum : %lld ,Count : %lld \n", threadResults[0].first,threadResults[0].second);
        
        end = timeSinceEpochMillisec();


        double singleProcessTime = (end - start)/1000.0;
        printf("time taken by Single Process: %f s\n\n\n\n", singleProcessTime);
        return singleProcessTime;

    }
    void MultiThreadingBPT::multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset){
        long long sum = 0;
        long long c = 0;

       bpt::bplus_tree database(DB_NAME);
        bpt::leaf_node_t temp;
        database.run_map(&temp, start_leaf_offset);
        while (temp.next != end_leaf_offset && c<10000){
            for (size_t i = 0; i < temp.n; ++i){
                sum += temp.children[i].value;
                c++;
            }
            // thread_starts[thread_number] = temp.next;
            database.run_map(&temp, temp.next);
        }
        if(end_leaf_offset==0){
            for (size_t i = 0; i < temp.n; ++i){
                sum += temp.children[i].value;
                c++;
            }
        }

        threadResults[thread_number] = {sum, c};
        thread_starts[thread_number] = temp.next;


        // printf("threadNum: %d sum %lld, count: %d\n", thread_number,sum, c);
        threadResults[thread_number] = {sum, c};
    }