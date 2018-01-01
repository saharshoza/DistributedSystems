#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
 
class SharedQueue
{
 public:
 	SharedQueue(){};
 	~SharedQueue(){};
 	void pop();
 	char* front();
 	void push(char* data);
private:
	std::queue<char*> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};