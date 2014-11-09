#pragma once

#include <list>

#include "core/GloveFwd.h"

namespace glove {

class IGameComponentFactory {
public:
    virtual ~IGameComponentFactory() {};

    virtual GameComponentPtr Build(const GameObjectPtr owner) const = 0;
};

} /* namespace glove */