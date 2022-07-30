#include<vector>
#include<deque>
#include<thread>
#include<mutex>
#include<functional>
#include<semaphore.h>
using namespace std;

struct ThreadPool {
    ThreadPool(int num);
    void execute(function<void()> task);
    void wait() {
        for (auto& t: workers) t.join();
    }
    void stop() {
        fstop = 1;
        for (int i = 1; i <= num; i++) sem_post(&if_has);
        wait();
    }
private:
    int num = 0;
    mutex m;
    bool fstop = 0;
    sem_t if_has, if_full;
    vector<thread> workers;
    deque<function<void()>> tasks;
};

ThreadPool::ThreadPool(int num): num(num) {
    sem_init(&if_has, 0, 0);
    sem_init(&if_full, 0, 50);
    auto handle = [this](int id) {
        while (1 && !fstop) {
            sem_wait(&if_has);
            if (fstop) break;
            m.lock();
            auto task = tasks.front(); tasks.pop_front();
            m.unlock();
            sem_post(&if_full);
            task();
        }
        printf("thread%d has stopped!!!!\n", id);
    };
    for (int i = 1; i <= num; i++) {
        workers.emplace_back(handle, i);
    }
}
void ThreadPool::execute(function<void()> task) {
    sem_wait(&if_full);
    m.lock();
    tasks.push_back(task);
    m.unlock();
    sem_post(&if_has);
}   