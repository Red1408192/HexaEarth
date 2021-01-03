#pragma once
#include <thread>
#include <mutex>
#include <vector>
#include <future>
#include <iostream>
#include <queue>
#include <functional>
#include <tuple>
#include <map>
using namespace std;

mutex resultQueueMutex;

template<typename R>
class ResultQueue{
    vector<pair<int, R>> ResultSet;
    int count = 0;
    public:
        void AddToQueue(R res, int index){
            unique_lock<mutex> lock(resultQueueMutex);
            ResultSet.emplace_back(index, res);
            count++;
        }

        R operator[](int key){
            if(ResultSet.count(key)>0){
                unique_lock<mutex> lock(resultQueueMutex);
                R _ret = ResultSet[key];
                count--;
                return _ret;
            }
        }

        bool await(int indexToWait, R& placer){
            unique_lock<mutex> lock(resultQueueMutex);
            for(pair<int, R> set : ResultSet){
                if(set.first != indexToWait){
                    continue;
                }
                placer = set.second;
                count--;
                return true;
            }
            return false;
        }
};

mutex poolMutex;
mutex queueMutex;

template<typename R, typename ... T>
class ThreadPool{
    vector<thread> Pool;
    queue<function<R(T...)>> opQueue;
    queue<tuple<T...>> argQueue;
    ResultQueue<R> resQue;

    bool killPool = false;
    int jobCounter = 1;
    int processCounter = 1;

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
            cout<<"Thread pool size "<< tNum <<" initialized\n";
        }

        int AddJob(function<R(T...)> newJob, T... args){
            cout<<"Adding "<< jobCounter <<"# job to queue\n";
            int index;
            {            
                unique_lock<mutex> lock(queueMutex);
                opQueue.push(newJob);
                argQueue.push(tuple<T...>(args...));
                index = jobCounter;
                jobCounter++;
            }
            return index;
        }

        ResultQueue<R>& Subscribe(){
            if constexpr (!is_same<R,void>()){
                return resQue;
            }
            throw;
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
                int thisjobIndex;
                {
                    unique_lock<mutex> lock(queueMutex);
                    if(!opQueue.empty()){
                        cout<<"Begining "<< processCounter<<"# job\n";
                        thisjobIndex = processCounter;
                        processCounter ++;
                        operation = &opQueue.front();
                        arguments = &argQueue.front();
                        opQueue.pop();
                        argQueue.pop();
                        isProcessing = true;
                    }
                }
                if(isProcessing){
                    if constexpr (is_same<void, R>::value){ //constexpr = "compile time if"
                        //apply(operation, &arguments); //**apply does not work on templated functions
                        apply([&operation](T... tArg){(*operation)(tArg...);}, *arguments);
                        cout<<"Job #"<< thisjobIndex <<"\n";
                    }
                    else{
                        R retObj = apply([&operation](T... tArg){return (*operation)(tArg...);}, *arguments);
                        cout<<"Job #"<< thisjobIndex <<" done\n";
                        resQue.AddToQueue(retObj, thisjobIndex);
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