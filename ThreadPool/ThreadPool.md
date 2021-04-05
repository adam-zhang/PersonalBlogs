# Thread Pool

[toc]

1. Defintion:
   
In a project, sometimes we have many tasks to deal with in many threads, but there's a problem is that we need to create and destroy a thread which takes time, if we just create many threads and suspend them until the tasks come and assign tasks for them one by one, and destroy the threads when application ends, it should save lot of time. In this situation, we put many threads in a container called ThreadPool.

2. Key technicles:

The key is condition_variable, in Windows it would be event, when you created a thread which needs a callback function, in this callback function we should let thread suspended, when task comes then active thread one by one. the code should be like below:

```
void work()
{
	Task task;
	while(!tasks_.empty())
	{
		task = tasks_.front();
		tasks_.pop();
	}
	if (task)
		task();
}
```


