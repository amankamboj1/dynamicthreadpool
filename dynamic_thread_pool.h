#ifndef DYNAMIC_THREAD_POOL_H
#define DYNAMIC_THREAD_POOL_H

#include <atomic>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>

// Default thread count
const int kThreadCount = 5;

/**
 * @class DynamicThreadPool
 * @brief creates and maintains a thread pool.
 * @details We are using C++11 threads to create a thread pool
 */
class DynamicThreadPool {
  /**
   * @class ScopedThread
   * @brief Manages the lifetime of threads
   */
  class ScopedThread {
   public:
    /**
     * ScopedThread Constructor.
     * @param objThread thread object to be managed. After a call to this, the
     * supplied thread object will represent <b><em>not-a-thread</em></b>.
     */
    explicit ScopedThread(std::thread objThread);

    /**
     * ScopedThread Constructor.
     * @param rhs r-value reference to ScopedThread object.  After a call to
     * this, the supplied thread object wrapped in <em>rhs</em> will represent
     * not-a-thread
     */
    ScopedThread(ScopedThread &&rhs);

    /**
     * ScopedThread Destructor.
     * @details This will wait for the owned thread to join, but only if it is
     * joinable.
     */
    ~ScopedThread();

    /**
     * ScopedThread Deleted Copy Constructor
     */
    ScopedThread(const ScopedThread & /*rhs*/) = delete;
    /**
     * ScopedThread Deleted Move Assignment Operator.
     */
    ScopedThread &operator=(ScopedThread && /*rhs*/) = delete;
    /**
     * ScopedThread Deleted Copy Assignment Operator.
     */
    ScopedThread &operator=(const ScopedThread & /*rhs*/) = delete;

   private:
    std::thread m_thread;
  };

 public:
  /**
   * DynamicThreadPool Constructor
   * @param thread count which defaults to kThreadCount - 5
   */
  DynamicThreadPool(unsigned int threadCount = kThreadCount);

  /**
   * DynamicThreadPool Default Destructor
   */
  ~DynamicThreadPool();

  /**
   * Pushes the task (function) that needs to be performed by thread pool.
   * As the tasks are stored by priority in map, they shall be executed in that order.
   * @param priority of the task to be performed
   * @return void
   */
  void Push(unsigned int priority, std::function<void()> task);

  /**
   * Increases or decreases the number of threads in thread pool
   * @param new number of threads
   * @return true if success else failure
   */
  bool SetThreadPoolSize(const unsigned int &newThreadCount);

  /**
   * Returns number of threads in thread pool
   * @return number of threads in thread pool
   */
  unsigned int GetThreadCount();

  /**
   * Returns number of pending jobs in thread pool
   * @return number of pending jobs in thread pool
   */
  unsigned int GetPendingJobsCount();

 private:
  void StartThreads(const unsigned int &threadsToStart);
  void StopThreads();
  void Work();

  // Variables
  std::deque<ScopedThread> m_threadContainer;
  std::multimap<unsigned int, std::function<void()>> m_jobsContainer;
  std::mutex m_jobsContainerMutex;
  std::mutex m_threadContainerMutex;
  std::condition_variable m_condtionVar;
  std::atomic<bool> m_poolActive{};
};

#endif /* DYNAMIC_THREAD_POOL_H */
