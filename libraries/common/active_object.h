#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H

#include <thread>
#include <functional>
#include <memory>

#include <common/shared_queue.h>
#include <common/noncopyable.h>

class ActiveObject : public NonCopyable
{
public:
  typedef std::function<void()> Callback;

public:
   virtual ~ActiveObject()
   {
      send([this] { iDone = true; });
      iThread.join();
   }

   void send(Callback aMessage) { iQueue.push(aMessage); }

   static std::unique_ptr<ActiveObject> create()
   {
      std::unique_ptr<ActiveObject> obj(new ActiveObject());
      obj->iThread = std::thread(&ActiveObject::run, obj.get());
      return obj;
   }

private:
   ActiveObject() CFG_DECL_DEFAULT;

   void run()
   {
      while (!iDone)
      {
         Callback func;
         iQueue.pop(func);
         func();
      }
   }

private:
   SharedQueue<Callback>  iQueue;
   std::thread            iThread;
   bool                   iDone = false; 
};

#endif // ACTIVE_OBJECT_H
