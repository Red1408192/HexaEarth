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

mutex poolMutex;
mutex queueMutex;

template<typename R, typename ... T>
class ThreadPool{
    vector<thread> Pool;
    queue<function<R(T...)>> opQueue;
    queue<tuple<T...>> argQueue;

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
                opQueue.push(newJob);
                argQueue.push(tuple<T...>(args...));
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
                function<R(T...)> *operation;
                tuple<T...> *arguments;
                {
                    unique_lock<mutex> lock(queueMutex);
                    if(!opQueue.empty()){
                        cout<<"Begining job\n";
                        operation = &opQueue.front();
                        arguments = &argQueue.front();
                        opQueue.pop();
                        argQueue.pop();
                        isProcessing = true;
                    }
                }
                if(isProcessing){
                    if constexpr (is_same<void, R>::value){ //"compile time if"
                        apply(operation, arguments);
                    }
                    else{
                        R retObj = apply(operation, arguments);
                    }
                    isProcessing = false;
                }
            }
        }

        bool IsQueEmpty(){
            return opQueue.empty();
        }

        int ThreadsCount(){
            return Pool.size();
        }
};