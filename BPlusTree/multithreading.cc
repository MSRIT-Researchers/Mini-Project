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
std::pair<long long, long long> threadResults[100];

void multithread(int left, int right, const int threadNumber);
void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset);

using namespace bpt;
bplus_tree database(DB_NAME);

int main(void)
{

    int i;
    meta_t meta = database.get_meta();
    int number_of_threads = meta.number_of_threads;
    std::thread threads[meta.number_of_threads];

    leaf_node_t leaf;

    

    printf("Number of Threads: %ld \n\n",meta.number_of_threads );
    for(int i=0; i<meta.number_of_threads; ++i){
         database.run_map(&leaf, meta.thread_offsets[i]);
         printf("Thread %d offset : %d\n",i, leaf.children[0].value);
    }   
    printf("\n");

    clock_t start, end;

    start = clock();

    threads[0] = std::thread(multithread_aggregate,0, meta.thread_offsets[0], 0);
    if(threads[0].joinable())
        threads[0].join();
    
    end = clock();
    printf("Sum: %d, count: %d\n", threadResults[0].first, threadResults[0].second);

    double stt = (end - start) / (double)(CLOCKS_PER_SEC);
    printf("time taken by SingleThread: %f s\n", (end - start) / (double)(CLOCKS_PER_SEC));


    std::vector<std::future<void>> futures;
    start = clock();
    for (i = 0; i < meta.number_of_threads - 1; ++i)
    {
        futures.push_back(std::async(std::launch::async, [&](){
            multithread_aggregate( i, meta.thread_offsets[i], meta.thread_offsets[i + 1]);
        }));
        // threads[i] = std::thread(multithread_aggregate, i, meta.thread_offsets[i], meta.thread_offsets[i + 1]);
    }
    // threads[i] = std::thread(multithread_aggregate_last, i, meta.thread_offsets[i]);


    for (i = 0; i < meta.number_of_threads-1; ++i)
    {
        // if (threads[i].joinable())
        //     threads[i].join();
        futures[i].get();
        //printf("Thread %d : | Sum : %lld | No of Records : %lld\n",i, threadResults[i].first, threadResults[i].second);
    }
    end = clock();
    printf("time taken by Multithread: %f s\n", (end - start) / (double)(CLOCKS_PER_SEC));
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

    // printf("sum : %lld\n", sum);
    threadResults[thread_number] = {sum, c};
}

void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset)
{
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

    threadResults[thread_number] = {sum, c};
}
