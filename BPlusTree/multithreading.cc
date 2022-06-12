#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include "bpt.h"
#include "variables.h"

std::pair<long long, long long> threadResults[THREAD_NUM];

void multithread(int left, int right, const int threadNumber);
void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset);


using namespace bpt;

int main(void)
{
    int i;
    bplus_tree database(DB_NAME);
    meta_t meta = database.get_meta();
    std::thread threads[THREAD_NUM];
    std::string str;
    
    for (i = 0; i < meta.number_of_threads-1; ++i)
    {
        threads[i] = std::thread(multithread_aggregate, i, meta.thread_offsets[i], meta.thread_offsets[i+1]);        
    }
    threads[i] = std::thread(multithread_aggregate_last, i, meta.thread_offsets[i], 0);

    for (i = 0; i < meta.number_of_threads; ++i)
    {
        if (threads[i].joinable())
            threads[i].join();
        printf("%lld %lld\n", threadResults[i].first, threadResults[i].second);
    }

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

    threadResults[thread_number] = {sum, c};
}

void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset)
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

    for (int i = 0; i < temp.n; ++i)
        {
            sum += temp.children[i].value;
            c++;
        }
        database.run_map(&temp, temp.next);

    threadResults[thread_number] = {sum, c};
}