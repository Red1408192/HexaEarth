#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <queue>
using namespace std;

class ThreadPool{
    vector<thread> Pool;
    mutex Queue_Mutex;
    queue<function<void()>> Queue;

    mutex pool_mutex;
    bool terminate_pool;

    ThreadPool(int Tnum){
        for(int i = 0; i<Tnum; i++){
            Pool.push_back(thread(Infinite_loop_function));
        }
    }

    void Infinite_loop_function(){
        while(true)
        {
            unique_lock<mutex> lock(Queue_Mutex);

            auto Job = Queue.front();
            Queue.pop();
            Job();
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
        for(thread t : Pool){
            t.join();
        }
        Pool.clear();
    }
};