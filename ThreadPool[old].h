#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <iostream>
#include <queue>
#include <functional>
#include <tuple>
using namespace std;

template<typename R, typename ... T>
struct Func{
    function<R(T...)> *operation;
    tuple<T...> args;
    Func(function<R(T...)> op, T... ar) : operation(&op), args(ar ...){

    }
};

mutex poolMutex;
mutex queueMutex;

template<typename R, typename ... T>
class ThreadPool{
    vector<thread> Pool;
    queue<Func<R,T...>> Queue;
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

        void AddJob(function<R(T...)> newJob, T... args){
            cout<<"Adding job to queue\n";
            {            
                unique_lock<mutex> lock(queueMutex);
                Queue.push(Func<R,T...>(newJob, args...));
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
                Func<R, T...> *task;
                {
                    unique_lock<mutex> lock(queueMutex);
                    if(!Queue.empty()){
                        cout<<"Begining job\n";
                        task = &Queue.front();
                        Queue.pop();
                        isProcessing = true;
                    }
                }
                if(isProcessing){
                    if constexpr (is_same<void, R>::value){ //"compile time if"
                        apply(*task->operation, task->args);
                    }
                    else{
                        R retObj = apply(*task->operation, task->args);
                    }
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