## dynamicthreadpool
Creates a dynamic thread pool based on C++11 which can increase or decrease the number of threads.
The default thread count is 5.

# Inserting in pool
The `push` function inserts a task with a priority and function to insert in the thread pool.
As more tasks are inserted, the task with next hightest priority always takes precedence.

# Increasing size
Use `SetThreadPoolSize` and put a higher value to increase the threads. More threads will be added in the pool.

# Decreasing size
Use `SetThreadPoolSize` and put a lower value to decrease the threads. Here, all the existing tasks are completed first so that there is no data loss. Then the threads are stopped and new lower number of threads are created.
