#ifndef TOUR_H
#define TOUR_H

#include <iostream>
#include <semaphore>
#include <mutex>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdio>

using namespace std;

class Tour {
private:
    int groupSize;                             
    int tourGuidePresent;                     
    int maxThreads;                            
    std::binary_semaphore binaryLock{1};       
    std::binary_semaphore binaryLock_arrive{1};
    std::binary_semaphore binaryLock_leave{1};
    std::counting_semaphore<> groupSemaphore;  
    std::counting_semaphore<> groupLockS;      

    bool isTourActive = false;                 
    bool itiswrittten = false;                 
    int activeVisitors = 0;                    
    std::mutex counterMutex;                  
    std::thread::id lastThreadId;              

public:
    // Constructor
    Tour(int groupSize, int tourGuidePresent)
        : groupSize(groupSize),
          tourGuidePresent(tourGuidePresent), 
          maxThreads(tourGuidePresent == 1 ? (groupSize + 1) : groupSize),
          groupSemaphore(maxThreads),
          groupLockS(maxThreads) {
        
        if (groupSize <= 0) {
            throw std::invalid_argument("First argument (groupSize) must be a positive integer.");
        }
        if (tourGuidePresent != 0 && tourGuidePresent != 1) {
            throw std::invalid_argument("Second argument (tourGuidePresent) must be either 0 or 1.");
        }
    }

    // Destructor
    ~Tour() = default;

    
    void start();

    void arrive() {
        binaryLock_arrive.acquire();
        auto threadId = std::this_thread::get_id();

        groupLockS.acquire();                
        groupSemaphore.acquire();           

        {
            std::lock_guard<std::mutex> lock(counterMutex);
            ++activeVisitors;
            
            if (tourGuidePresent == 1) {
                lastThreadId = threadId;
            }
        }

        printf("Thread ID: %ld | Status: Arrived at the location.\n", std::hash<std::thread::id>{}(threadId));

        if (activeVisitors < groupSize) {
            printf("Thread ID: %ld | Status: Only %d visitors inside, starting solo shots.\n", std::hash<std::thread::id>{}(threadId), activeVisitors);
        }

       
        if (activeVisitors == groupSize && !itiswrittten) {
            printf("Thread ID: %ld | There are enough visitors, the tour is starting.\n", std::hash<std::thread::id>{}(threadId));
            isTourActive = true;
            itiswrittten = true;
            if (tourGuidePresent == 1) {
                printf("Thread ID: %ld | Status: Tour guide speaking, the tour is over.\n", std::hash<std::thread::id>{}(threadId));
            }
        }

        binaryLock_arrive.release();
    }

    void leave() {
        binaryLock_leave.acquire();
        auto threadId = std::this_thread::get_id();

        {
            std::lock_guard<std::mutex> lock(counterMutex);
            --activeVisitors;
        }

        if (tourGuidePresent == 1 && threadId == lastThreadId) {
           
            printf("Thread ID: %ld | Status: Tour guide has finished. All visitors have left, the new visitors can come.\n", std::hash<std::thread::id>{}(threadId));
            isTourActive = false;
            itiswrittten = false;
        } else if (isTourActive) {
            printf("Thread ID: %ld | Status: I am a visitor and I am leaving.\n", std::hash<std::thread::id>{}(threadId));
        } else {
            printf("Thread ID: %ld | Status: My camera ran out of memory while waiting, I am leaving.\n", std::hash<std::thread::id>{}(threadId));
        }

       
        if (activeVisitors == 0 && !(tourGuidePresent == 1 && threadId == lastThreadId)) {
            binaryLock.acquire(); 
            for (int i = 0; i < groupSize; ++i) {
                groupLockS.release();
            }
            binaryLock.release(); 

            if (isTourActive) {
                printf("Thread ID: %ld | Status: All visitors have left, the new visitors can come.\n", std::hash<std::thread::id>{}(threadId));
            }
            isTourActive = false;
            itiswrittten = false;
        }

        groupSemaphore.release(); 
        binaryLock_leave.release();
    }
};

#endif // TOUR_H
