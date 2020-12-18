#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <queue>
using namespace std;

mutex pool_mutex;
mutex Queue_Mutex;
class ThreadPool{
    vector<thread> Pool;
    queue<function<void()>> Queue;
    bool terminate_pool;

    public:
        ThreadPool(int tNum = 0){
            if (tNum == 0){
                tNum = thread::hardware_concurrency()/2;
            }
            for(int i = 0; i<tNum; i++){
                Pool.emplace_back(&ThreadPool::Process, this);
            }
        }

        void Add_Job(function<void()> New_Job){
            unique_lock<mutex> lock(Queue_Mutex);
            Queue.push(New_Job);
        }

        void shutdown(){
            unique_lock<mutex> lock(pool_mutex);
            terminate_pool = true;

            // Join all threads.
            for(thread &t : Pool){
                t.detach();
            }
            Pool.clear();
        }

        bool IsQueEmpty(){
            return Queue.size() > 0;
        }

    void Process(){
        while(true)
        {
            unique_lock<mutex> lock(Queue_Mutex);

            auto Job = Queue.front();
            Queue.pop();
            Job();
        }
    }
};