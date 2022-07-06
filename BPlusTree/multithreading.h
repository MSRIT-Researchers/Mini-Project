struct mesg_buffer {
            long long sum;
            long long count;
};

class MultiThreadingBPT{
    private:
        std::pair<long long, long long> threadResults[100];
    public: 
        
        void sendDataToMessageQ(long long sum, long long count);
        void listenToMessageQ();
        void multithread(int left, int right, const int threadNumber);
        void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
        void multithread_aggregate_single(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset);
        void spawnChild(int i,int a , int  b);
        double computeUsingSingleProcess();
        double computeUsingMultipleProcesses();
        uint64_t timeSinceEpochMillisec();

        MultiThreadingBPT(bool single=false);
};