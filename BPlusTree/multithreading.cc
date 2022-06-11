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

using namespace bpt;

int main(void)
{
    int i;
    bplus_tree database(DB_NAME);
    meta_t meta = database.get_meta();
    std::thread threads[THREAD_NUM];
    // number_of_threads to be fetched from meta
    // std::thread threads[number_of_threads];

    std::string str;

    /*
    for (i = 0; i < THREAD_NUM; ++i)
    {
        threads[i] = std::thread(multithread, RANGE * i, RANGE * i + RANGE - 1, i);
    }

    for (i = 0; i < THREAD_NUM; ++i)
    {
        if (threads[i].joinable())
            threads[i].join();
        printf("%lld %lld\n", threadResults[i].first, threadResults[i].second);
    }

    /*
    for (int i = 0; i < meta.number_of_threads-1; ++i)
    {
        threads[i] = std::thread(multithread_aggregate, i, meta.thread_offsets[i], meta.thread_offsets[i+1]);
    }
    threads[i] = std::thread(multithread_aggregate, i, meta.thread_offsets[i]);

    for (int i = 0; i < meta.number_of_threads; ++i)
    {
        if (threads[i].joinable())
            threads[i].join();
        printf("%lld %lld\n", threadResults[i].first, threadResults[i].second);
    }
    */

    return 0;
}

void multithread(int left, int right, const int threadNumber)
{

    std::string str = std::to_string(left);
    char leftStr[str.length()];
    strcpy(leftStr, str.c_str());
    str = std::to_string(right);
    char rightStr[str.size()];
    strcpy(rightStr, str.c_str());

    bplus_tree database(DB_NAME);

    bpt::key_t start(leftStr);
    value_t values[512];
    bool next = true;
    long long sum = 0;
    long long c = 0;
    while (next)
    {
        int ret = database.search_range(&start, rightStr, values, 512, &next);
        if (ret < 0)
            break;

        for (int i = 0; i < ret; i++)
        {
            c++;
            sum += values[i];
        }
    }
    threadResults[threadNumber] = {sum, c};
}


void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0)
{
    long long sum = 0;
    long long c = 0;

    bplus_tree database(DB_NAME);
    leaf_node_t temp;
    database.run_map(&temp, temp.prev);
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
