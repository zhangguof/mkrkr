#ifndef _SAFE_QUEUE_
#define _SAFE_QUEUE_
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template <typename T>
class SafeQueue
{
private:
	std::queue<T> data;
	std::mutex mut;
	std::condition_variable cond;

#define WAIT_NOEMPTY std::unique_lock<std::mutex> ulk(mut);\
	cond.wait(ulk,[this](){return !data.empty();});


public:
	SafeQueue(){}
	~SafeQueue(){}
	void push(T& new_val)
	{
		std::lock_guard<std::mutex> lk(mut);
		data.push(new_val);
		cond.notify_one();

	}
	void wait_and_pop(T& val)
	{
		std::unique_lock<std::mutex> ulk(mut);
		cond.wait(ulk,[this](){return !data.empty();});
		// WAIT_NOEMPTY
		val = data.front();
		data.pop();
	}
	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> ulk(mut);
		cond.wait(ulk,[this](){return !data.empty();});
		// WAIT_NOEMPTY
		auto p = std::make_shared<T>(data.front());
		data.pop();
		return p;
	}
	bool try_pop(T& val)
	{
		std::lock_guard<std::mutex> lk(mut);
		if(data.empty())
			return false;
		val = data.front();
		data.pop();
		return true;
	}
	bool empty()
	{
		std::lock_guard<std::mutex> lk(mut);
		return data.empty();
	}
	unsigned int size()
	{
		return data.size();
	}

};

#endif