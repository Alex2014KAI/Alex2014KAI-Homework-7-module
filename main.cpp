#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <condition_variable>
#include <functional>
#include <vector>

using namespace std;

// using FuncPtr = void(*)();
mutex global_mtx1;
mutex global_mtx2;

template <typename T>
class safe_queue {
public:
    void push(T fn) {
        lock_guard<mutex> lg(mt); // We grab a mutex so that no one else can add a function to the thread.
        queue_.push(std::move(fn));
        cv.notify_one();
    }

    T pop() {
        unique_lock<std::mutex> ul(mt);
        cv.wait(ul, [this]() { return !queue_.empty(); });
        auto func = std::move(queue_.front());
        queue_.pop();
        return func;
    }
private:
    mutex mt;
    condition_variable cv;
    std::queue<T> queue_;
};


class thread_pool {
public:
    thread_pool() {
        numberThreads = _Thrd_hardware_concurrency();
        for (int i{ 0 }; i < (numberThreads - 3); i++) {
            vectorThread.push_back(thread(&thread_pool::work, this));
            // vectorThread.push_back(thread());
        }
    };

    ~thread_pool() {
        for (int i{ 0 }; i < (numberThreads - 3); i++) {
            vectorThread[i].join();
        }
    };

    void work() {
        std::function<void()> currentTask;
        unique_lock<mutex> ul1(global_mtx1);
        cout << "Start working thread id:" << this_thread::get_id() << endl;
        ul1.unlock();
        while (true) {
            unique_lock<mutex> ul2(global_mtx2);
            currentTask = queueFunction.pop();
            cout << "Working thread id:" << this_thread::get_id() << endl;
            currentTask();
            
        }
    };

    void submit(std::function<void()> fn, int count) {
        std::cout << "Add function:  " << count << std::endl;
        queueFunction.push(move(fn));
    }


private:
    int numberThreads;
    vector<thread> vectorThread;
    safe_queue<std::function<void()>> queueFunction;


};

// Примеры функций
void fo1() {
    std::cout << "Work fo1" << std::endl;
}

void fo2() {
    std::cout << "Work fo2" << std::endl;
}
void fo3() {
    std::cout << "Work fo3" << std::endl;
}

void fo4() {
    std::cout << "Work fo4" << std::endl;
}
void fo5() {
    std::cout << "Work fo5" << std::endl;
}

void fo6() {
    std::cout << "Work fo6" << std::endl;
}
// Конец примеров функций


int main()
{

    vector < std::function<void()> > vecFunc{fo1, fo2, fo3, fo4, fo5, fo6};
    int coutnFn = 0;
    
    thread_pool thread_pool_;
    
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        thread_pool_.submit(vecFunc[coutnFn], coutnFn);
        thread_pool_.submit(vecFunc[coutnFn + 1], coutnFn + 1);
        if (coutnFn == 4) { coutnFn = 0; };
         coutnFn+=2;
    }

}