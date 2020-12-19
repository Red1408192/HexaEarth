#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <iostream>
#include <queue>
#include <functional>
using namespace std;

mutex poolMutex;
mutex queueMutex;
class ThreadPool{
    vector<thread> Pool;
    queue<function<void()>> Queue;
    bool killPool = false;

    public:
        ThreadPool(int tNum = 0){
            if (tNum == 0){
                tNum = thread::hardware_concurrency()-2;
            }
            for(int i = 0; i<tNum+1; i++){
                {                
                    unique_lock<mutex> lock(poolMutex);
                    Pool.emplace_back(&ThreadPool::Process, ref(*this));
                }
            }
            cout<<"Thread pool initialized\n";
        }

        void AddJob(function<void()> newJob){
            cout<<"Adding job to queue\n";
            {            
                unique_lock<mutex> lock(queueMutex);
                Queue.push(newJob);
            }
        }

        void shutdown(){

            unique_lock<mutex> lock(poolMutex);
            killPool = true;

            // Join all threads.
            for(thread &t : Pool){
                t.join();
            }
            Pool.clear();
        }

        void Process(){
            bool isProcessing = false;
            while(!killPool)
            {
                function<void()> Job;
                {
                    unique_lock<mutex> lock(queueMutex);
                    if(!Queue.empty()){
                        cout<<"Begining job\n";
                        Job = Queue.front();
                        Queue.pop();
                        isProcessing = true;
                    }
                }
                if(isProcessing){
                    Job();
                    isProcessing = false;
                }
            }
        }

        bool IsQueEmpty(){
            return Queue.empty();
        }

        int ThreadsCount(){
            return Pool.size();
        }
};