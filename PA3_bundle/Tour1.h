// Tour.h
#ifndef TOUR_H
#define TOUR_H

#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <stdexcept> 
#include <sstream>
#include <cstdio>
#include <mutex>
#include <semaphore>

using namespace std;

class ConcurrentCounter {
private:
    int value;           // Counter değeri
    std::mutex counterMutex; // Mutex ile thread-safe erişim

public:
    // Constructor: Counter başlangıç değeri sıfır
    ConcurrentCounter() : value(0) {}

    // Increment metodu: Değeri artırır
    void increment() {
        std::lock_guard<std::mutex> lock(counterMutex);
        ++value;
    }

    // Decrement metodu: Değeri azaltır
    void decrement() {
        std::lock_guard<std::mutex> lock(counterMutex);
        --value;
    }

    // getValue metodu: Mevcut değeri döndürür
    int getValue() {
        std::lock_guard<std::mutex> lock(counterMutex);
        return value;
    }
};

ConcurrentCounter counter;

class Tour {
private:
    
    int groupSize;           
    int hasGuide;     
    bool isTourActive;
    bool itiswrittten;

    binary_semaphore binaryLock;
     



public:
    // Constructor
    Tour(int groupSize, int tourGuidePresent){
        // Giriş argümanlarının geçerliliğini kontrol et
        if (groupSize <= 0) {
            throw invalid_argument("Group size must be a positive integer.");
        }
        if (hasGuide != 0 && hasGuide != 1) {
            throw invalid_argument("Second argument must be either 0 (no guide) or 1 (guide present).");
        }

        this->groupSize = groupSize;
        this->hasGuide = tourGuidePresent;
        this->isTourActive = false;
       

        // Semaforu grup boyutuna göre başlat bunu sonra tanımla
        
    }


    // Destructor
    ~Tour(){
        // semofore ve threadleri serbets bıraksın
    }

    // Metotlar
    void arrive(){
    
    }
    void leave(){

    }
    void start();   // Turun başladığını belirtir
};

#endif // TOUR_H


/*

#include <cstdio>
#include <thread>
#include <vector>
#include <semaphore>
#include <chrono>
#include <iostream>
#include <mutex>



constexpr int DEFAULT_THREADS = 4;

bool isTourActive = false;
bool itiswrittten = false;
constexpr bool isGuide = true; 

#define MAX_THREADS (isGuide ? (DEFAULT_THREADS + 1) : DEFAULT_THREADS)


std::binary_semaphore binaryLock(1);

// Semaphore tanımları
std::counting_semaphore<MAX_THREADS> groupSemaphore(MAX_THREADS); // Gruplar için semaphore
//std::binary_semaphore groupLock(1);     // Grup tamamlanmasını kontrol eden binary semaphore
std::counting_semaphore<MAX_THREADS> groupLockS(MAX_THREADS);

class ConcurrentCounter {
private:
    int value;           // Counter değeri
    std::mutex counterMutex; // Mutex ile thread-safe erişim

public:
    // Constructor: Counter başlangıç değeri sıfır
    ConcurrentCounter() : value(0) {}

    // Increment metodu: Değeri artırır
    void increment() {
        std::lock_guard<std::mutex> lock(counterMutex);
        ++value;
    }

    // Decrement metodu: Değeri azaltır
    void decrement() {
        std::lock_guard<std::mutex> lock(counterMutex);
        --value;
    }

    // getValue metodu: Mevcut değeri döndürür
    int getValue() {
        std::lock_guard<std::mutex> lock(counterMutex);
        return value;
    }
};

ConcurrentCounter counter;


void workerTask(int id) {
    //printf("it is just begging for %d\n",id);
    groupLockS.acquire();
    groupSemaphore.acquire(); // Grupla aynı anda en fazla 4 thread çalışabilir
    counter.increment();
    //groupLockS.acquire();

    // Thread işlevi
    
    printf("Thread %d is running. ad counter is : %d\n", id, counter.getValue());
    if(counter.getValue() == MAX_THREADS && !itiswrittten){
        printf("There are enough visitors, the tour is starting.\n");
        isTourActive = true;
        itiswrittten = true;
        if(isGuide){
            printf("Tour guide speaking, the tour is over\n");
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(2)); // İş simülasyonu
    //printf("Thread %d has finished. and counter is: %d\n", id, counter.getValue());
    
    counter.decrement();
    
    if(isTourActive){
        printf("Thread %d has finished. and counter is: %d\n", id, counter.getValue());
    }else {
        printf("Thread %d Status: My camera ran out of memory while waiting, I am leaving.\n", id);
    }
    
    if(counter.getValue() == 0){
        // bu release kodu bu şekilde olmamalı yani sayıya göre dinamik olması gerekiyor
        binaryLock.acquire();
        for(int i = 0; i < MAX_THREADS; i++){
            groupLockS.release();
        }
        binaryLock.release();
        
        //groupLockS.release();
        //groupLockS.release();
        //groupLockS.release();
        //groupLockS.release();
        if(isTourActive){
            printf(" All visitors have left, the new visitors can come.\n");
        }
        isTourActive = false;
        itiswrittten = false;
        
    }
    // if li printf eskiden burdaydı
    
    groupSemaphore.release(); // Thread işini tamamladı, hakkını bırak

    // Eğer bu, grubun son thread'iyse
    
}

int main() {
    
    constexpr int TOTAL_THREADS = 10; // Toplam thread sayısı
    std::vector<std::thread> threads;
    

    for (int i = 0; i < TOTAL_THREADS; i++) {
        // Her gruptan sonra yeni grubun başlamasını bekle
        printf("Thread %d is born.\n", i);
        threads.emplace_back(workerTask, i);
    }

    // Tüm thread'lerin bitmesini bekle
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    printf("Termination is finished.\n");

    return 0;
}
*/

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
    int groupSize;                             // Grubun minimum büyüklüğü
    bool hasGuide;                             // Rehberin mevcut olup olmadığını belirtir
    int maxThreads;                            // Grup boyutuna göre dinamik olarak belirlenir
    std::binary_semaphore binaryLock{1}; // Erişim kontrolü için binary semaphore
    std::binary_semaphore binaryLock_arrive{1}; 
     std::binary_semaphore binaryLock_leave{1}; 
    std::counting_semaphore<> groupSemaphore;  // Grup katılımcılarını sınırlar
    std::counting_semaphore<> groupLockS;      // Grup tamamlama kontrolü

    bool isTourActive = false;                 // Turun aktif olup olmadığını belirtir
    bool itiswrittten = false;                 // Tur mesajının yazıldığını takip eder
    int activeVisitors = 0;                    // Mevcut katılımcı sayısı
    std::mutex counterMutex;                   // Aktif ziyaretçi sayısını korur

public:
    // Constructor
    Tour(int groupSize, bool tourGuidePresent)
        : groupSize(groupSize), 
          hasGuide(tourGuidePresent), 
          maxThreads(tourGuidePresent ? (groupSize + 1) : groupSize),
          groupSemaphore(maxThreads),
          groupLockS(maxThreads) {
        if (groupSize <= 0) {
            throw invalid_argument("Group size must be a positive integer.");
        }
    }

    // Destructor
    ~Tour() = default;

    // Turun başladığını belirtmek için kullanılacak içi boş metot
    void start();

    void arrive() {
        binaryLock_arrive.acquire();
        
        auto threadId = this_thread::get_id();
        
        groupLockS.acquire();                // Grup kilidini kontrol et
        groupSemaphore.acquire();           // Grup boyutunu kontrol et

        {
            std::lock_guard<std::mutex> lock(counterMutex);
            ++activeVisitors;
        }
        
        printf("Thread ID:  %ld | Status: Arrived at the location.\n", std::hash<std::thread::id>{}(threadId));

        //cout << "Visitor has arrived. Current count: " << activeVisitors << endl;
        
        if (activeVisitors < groupSize){
            printf("Thread ID: %ld | Status: Only %d visitors inside, starting solo shots.\n", std::hash<std::thread::id>{}(threadId), activeVisitors);
        }

        // Tur için yeterli sayıda kişi varsa
        if (activeVisitors == groupSize && !itiswrittten) {
            printf("Thread ID: %ld | There are enough visitors, the tour is starting.\n", std::hash<std::thread::id>{}(threadId));
            isTourActive = true;
            itiswrittten = true;
            if (hasGuide) {
                printf("Thread ID: %ld | Status: Tour guide speaking, the tour is over.\n", std::hash<std::thread::id>{}(threadId));
            }
        }
        
        binaryLock_arrive.release();
    }

    void leave() {
        binaryLock_leave.acquire();
        {
            std::lock_guard<std::mutex> lock(counterMutex);
            --activeVisitors;
        }
        
         auto threadId = this_thread::get_id();

        if (isTourActive) {
            //cout << "A visitor left. Remaining visitors: " << activeVisitors << endl;
            printf("Thread ID:  %ld | Status: I am a visitor and I am leaving.\n", std::hash<std::thread::id>{}(threadId));
        } else {
            //cout << "Visitor leaving due to waiting issues." << endl;
            printf("Thread ID: %ld | Status: My camera ran out of memory while waiting, I am leaving.\n", std::hash<std::thread::id>{}(threadId));
        }

        // Eğer bu, grubun son ziyaretçisiyse
        if (activeVisitors == 0) {
            binaryLock.acquire(); // Kilitle
            for (int i = 0; i < groupSize; ++i) {
                groupLockS.release();
            }
            binaryLock.release(); // Kilidi aç

            if (isTourActive) {
                printf("Thread ID:  %ld | Status: All visitors have left, the new visitors can come.\n", std::hash<std::thread::id>{}(threadId));
            }
            isTourActive = false;
            itiswrittten = false;
        }

        groupSemaphore.release(); // Ayrılan kişi hakkını bırakır
        binaryLock_leave.release();
    }
};

#endif // TOUR_H
