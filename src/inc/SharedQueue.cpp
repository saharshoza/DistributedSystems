#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

#ifndef __SHAREDQUEUE_H_INCLUDED__ 
#define __SHAREDQUEUE_H_INCLUDED__
#include "../../include/SharedQueue.h"
#endif

// template <typename T>
void SharedQueue::pop()
{
  std::unique_lock<std::mutex> mlock(mutex_);
  if(!queue_.empty()){
    queue_.pop();
  }
  //printf("Process %d::CommInterface::Queue popped at time %lu with size %lu\n", (int) getpid(), std::time(0), queue_.size() );
  mlock.unlock();
  cond_.notify_one();
}
 
// template <typename T>
char* SharedQueue::front()
{
  std::unique_lock<std::mutex> mlock(mutex_);
  char* data;
  if(!queue_.empty()){
    data = queue_.front();
  }
  else{
    data = NULL;
  }
  //char* data = queue_.front();
  //printf("Process %d::CommInterface::Queue read %s at time %lu with size %lu\n", (int) getpid(), data, std::time(0), queue_.size() );
  mlock.unlock();
  cond_.notify_one();
  return data;
}

// template <typename T> 
void SharedQueue::push(char* data)
{
  std::unique_lock<std::mutex> mlock(mutex_);
  queue_.push(data);
  //printf("Process %d::Client::Queue pushed %s at time %lu with size %lu\n", (int) getpid(), data, std::time(0), queue_.size() );
  mlock.unlock();
  cond_.notify_one();
}

