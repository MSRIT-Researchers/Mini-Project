struct mesg_buffer {
            long long sum;
            long long count;
};
#include <thread>
class MultiThreadingBPT{
    private:
        std::pair<long long, long long> threadResults[100];
    public: 
        
        void sendDataToMessageQ(long long sum, long long count);
        void listenToMessageQ();
        void multithread(int left, int right, const int threadNumber);
        void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
        void multithread_aggregate_single(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset);
        std::thread spawnThread(int i,int a , int  b);
        double computeUsingSingleProcess();
        double computeUsingMultipleProcesses(bool openMP);
        uint64_t timeSinceEpochMillisec();

        MultiThreadingBPT(bool single, bool openMP);
};