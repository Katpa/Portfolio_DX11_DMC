#include "Framework.h"

ThreadPool::ThreadPool()
{
	FOR(4)
		threads.emplace_back(&ThreadPool::Tasking, this);
}

ThreadPool::~ThreadPool()
{
    unique_lock<mutex> lock(queueMutex);
    stop = true;
    condition.notify_all();
    for (thread& thread : threads)
        thread.join();
}

void ThreadPool::GetTask(ThreadBuffer buffer)
{
    {
        unique_lock<mutex> lock(queueMutex);
        tasks.emplace(move(bind(buffer.func, buffer.firstParam, buffer.secondParam, buffer.pathes, buffer.index)));
    }
    condition.notify_one();
}

void ThreadPool::Tasking()
{
    while (true) {
        Event task;
        {
            unique_lock<mutex> lock(queueMutex);
            condition.wait(lock, [this] { return stop || !tasks.empty(); });
            if (stop && tasks.empty()) {
                return;
            }
            task = move(tasks.front());
            tasks.pop();
        }
        task();
    }
}
