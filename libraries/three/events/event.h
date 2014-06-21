#ifndef THREE_EVENTS_EVENT_H
#define THREE_EVENTS_EVENT_H

#include <three/common.h>

#include <three/constants.h>

namespace three {

class Event {
public:

  Event( const EventType& typeIn, void* targetIn = nullptr )
  : type( std::move( typeIn ) ),
    target( targetIn ) {}
    
  const EventType type;

  void* target;

};

} // end namespace three

#endif // THREE_EVENTS_EVENT_H