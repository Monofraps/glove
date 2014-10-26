#pragma once

#include "pitamem/MemoryProfile.h"

#include "event/type/KeyEvent.h"
#include "event/type/MouseButtonEvent.h"
#include "event/type/MouseMoveEvent.h"

namespace glove {

class EventSubscriber {
Profilable()
public:
    EventSubscriber();

    virtual ~EventSubscriber();

    virtual void OnKeyEvent(const KeyEvent& evnt) {    };

    virtual void OnMouseButtonEvent(const MouseButtonEvent& evnt) {    };

    virtual void OnMouseMoveEvent(const MouseMoveEvent& evnt) {    };
};


} // namespace glove