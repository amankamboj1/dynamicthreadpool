#include "dynamic_thread_pool.h"
#include <stdexcept>

DynamicThreadPool::ScopedThread::ScopedThread(std::thread objThread) : m_thread{std::move(objThread)} {}

DynamicThreadPool::ScopedThread::ScopedThread(ScopedThread &&rhs) : m_thread{std::move(rhs.m_thread)} {}

DynamicThreadPool::ScopedThread::~ScopedThread() {
  if (m_thread.joinable()) {
    m_thread.join();
  }
}

DynamicThreadPool::DynamicThreadPool(unsigned int threadCount /*= kThreadCount*/) {
  if (threadCount == 0) {
    // Bad value provided
    throw std::invalid_argument("Thread count can not be 0");
    return;
  }

  StartThreads(threadCount);
}

DynamicThreadPool::~DynamicThreadPool() { StopThreads(); }

void DynamicThreadPool::StartThreads(const unsigned int &threadsToStart) {
  m_poolActive = true;
  std::lock_guard<std::mutex> guard(m_threadContainerMutex);
  // Threads are started and given infinite dummy work.
  for (unsigned int i = 0; i < threadsToStart; ++i) {
    m_threadContainer.push_back(ScopedThread{std::thread(&DynamicThreadPool::Work, this)});
  }
}

void DynamicThreadPool::Work() {
  std::function<void()> task;
  while (true) {
    std::unique_lock<std::mutex> lock(m_jobsContainerMutex);
    while (m_poolActive && m_jobsContainer.empty()) {
      m_condtionVar.wait(lock);
    }

    if (!m_poolActive) {
      return;
    }

    // Get the first item from map
    task = m_jobsContainer.begin()->second;
    m_jobsContainer.erase(m_jobsContainer.begin());
    lock.unlock();

    task();

    // Also check after task completion
    if (!m_poolActive) {
      return;
    }
  }
}

void DynamicThreadPool::Push(unsigned int priority, std::function<void()> task) {
  std::unique_lock<std::mutex> lock(m_jobsContainerMutex);
  m_jobsContainer.insert(std::pair<unsigned int, std::function<void()>>(priority, task));
  lock.unlock();

  // Wake up only one thread
  m_condtionVar.notify_one();
}

bool DynamicThreadPool::SetThreadPoolSize(const unsigned int &newThreadCount) {
  if (newThreadCount == 0) {
    return false;
  }

  unsigned int currentThreadCount = GetThreadCount();
  if (currentThreadCount < newThreadCount) {
    StartThreads(newThreadCount - currentThreadCount);  // Increase threads
  } else if (currentThreadCount > newThreadCount) {
    // Decrease the threads. This is done by resetting the worker thread.
    // This will not impact the tasks already in queue
    StopThreads();
    StartThreads(newThreadCount);
  }

  return true;
}

void DynamicThreadPool::StopThreads() {
  m_poolActive = false;
  m_condtionVar.notify_all();
  std::lock_guard<std::mutex> guard(m_threadContainerMutex);
  m_threadContainer.clear();
}

unsigned int DynamicThreadPool::GetThreadCount() {
  std::lock_guard<std::mutex> guard(m_threadContainerMutex);
  return (unsigned int)m_threadContainer.size();
}

unsigned int DynamicThreadPool::GetPendingJobsCount() {
  std::lock_guard<std::mutex> guard(m_jobsContainerMutex);
  return (unsigned int)m_jobsContainer.size();
}
