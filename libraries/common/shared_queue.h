#ifndef SHARED_QUEUE_H
#define SHARED_QUEUE_H

#include <common/noncopyable.h>

#include <chrono>
#include <condition_variable>
#include <exception>
#include <mutex>
#include <queue>

template<typename T>
class SharedQueue : public NonCopyable
{
public:
  void push(T aItem)
  {
    {
      std::lock_guard<std::mutex> lock(iMutex);
      iQueue.push(std::move(aItem));
    }

    iDataCondition.notify_one();
  }

  void pop(T& aItem)
  {
    std::unique_lock<std::mutex> lock(iMutex);
    iDataCondition.wait(lock, [this]() { return !iQueue.empty(); });
    moveAndPop(aItem);
  }

  bool pop_nowait(T& aItem)
  {
    std::lock_guard<std::mutex> lock(iMutex);

    if (iQueue.empty())
    {
      return false;
    }

    moveAndPop(aItem);

    return true;
  }

  template <class Rep, class Period>
  bool pop_wait_for(T& aItem, std::chrono::duration<Rep, Period> aWaitDuration)
  {
    std::unique_lock<std::mutex> lock(iMutex);
    if (iDataCondition.wait_for(lock, aWaitDuration, [this]() { return !iQueue.empty(); }))
    {
      moveAndPop(aItem);
      return true;
    }

    return false;
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lock(iMutex);
    return iQueue.empty();
  }

  std::size_t size() const
  {
    std::lock_guard<std::mutex> lock(iMutex);
    return iQueue.size();
  }

private:
  void moveAndPop(T& aItem)
  {
    aItem = std::move(iQueue.front());
    iQueue.pop();  
  }

private:
  std::queue<T>           iQueue;
  mutable std::mutex      iMutex;
  std::condition_variable iDataCondition;
};

#endif // SHARED_QUEUE_H
