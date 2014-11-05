#pragma once

#include <event/AbstractEvent.h>

namespace glove {

struct MouseMoveEvent : AbstractEvent {
    static const AbstractEvent::EventTypeId eventTypeId;

    MouseMoveEvent(double x, double y);

    virtual ~MouseMoveEvent();

    virtual const EventTypeId& GetEventType() const;

    double x;
    double y;
};


} // namespace glove