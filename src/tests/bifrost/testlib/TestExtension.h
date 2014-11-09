#pragma once

#include <core/GloveFwd.h>
#include <core/natex/ISystemExtension.h>

namespace glove {

class TestExtension : public ISystemExtension {

public:

    virtual ~TestExtension();

    virtual void RegisterSubsystems(const GloveCorePtr& engineCore);

    virtual const ExtensionUuid GetExtensionUuid() const;

    virtual const std::string& GetExtensionName() const;
};

} /* namespace glove */