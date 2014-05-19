#include "yaffut.h"

#include <common/shared_queue.hpp>

#include <thread>

struct SharedQueueTest {};

TEST(SharedQueueTest, should_push_value)
{
  SharedQueue<int> q;
  CHECK(q.empty());

  q.push(int());
  CHECK(!q.empty());
  EQUAL(1, q.size());
}

TEST(SharedQueueTest, should_pop_value)
{
  SharedQueue<int> q;
  q.push(int());

  int val;
  CHECK(q.pop_nowait(val));
  CHECK(q.empty());
  EQUAL(0, q.size());

  CHECK(!q.pop_nowait(val));
}

TEST(SharedQueueTest, should_block_consumer_thread)
{
  SharedQueue<int> q;
  const int expected = 10;

  std::thread consumer([&]()
  {
    int value = 0;
    q.pop(value);
    EQUAL(expected, value);
  });

  std::thread producer([&]()
  {
    q.push(expected);
  });

  producer.join();
  consumer.join();
}
