# Thread Pool

[toc]

1. Defintion:
   
In a project, sometimes we have many tasks to deal with in many threads, but there's a problem is that we need to create and destroy a thread which takes time, if we just create many threads and suspend them until the tasks come and assign tasks for them one by one, and destroy the threads when application ends, it should save lot of time. In this situation, we put many threads in a container called ThreadPool.


