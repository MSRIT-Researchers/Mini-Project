class MultiThreadingBPT{
    private:
        std::queue<int>& serverQ;
        std::pair<long long, long long> threadResults[100];
    public: 


        void multithread(int left, int right, const int threadNumber);
        void multithread_aggregate(const int thread_number, off_t start_leaf_offset, off_t end_leaf_offset = 0);
        void multithread_aggregate_last(const int thread_number, off_t start_leaf_offset);
        void spawnChild(int i,int a , int  b);
        double computeUsingSingleProcess();
        double computeUsingMultipleProcesses();
        uint64_t timeSinceEpochMillisec();

        MultiThreadingBPT(std::queue<int> &q );
};