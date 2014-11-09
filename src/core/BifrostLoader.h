#pragma once

#include <string>
#include <map>

#include <core/GloveFwd.h>
#include <core/ISharedLibraryLoaderFactory.h>
#include <core/ISystemExtensionLoader.h>
#include <core/ISystemExtension.h>
#include <log/Log.h>

namespace glove {

/// @brief The Bifrost loader is the default sys extension loader.
class BifrostLoader : public ISystemExtensionLoader {
  public:
    typedef std::pair<ISharedLibraryLoaderPtr, ISystemExtensionSharedPtr> SystemExtensionAndLoader;
    typedef std::map<ExtensionUuid, SystemExtensionAndLoader> SystemExtensionMap;

    BifrostLoader();
    virtual ~BifrostLoader() {}

    virtual ISystemExtensionPtr LoadSystemExtension(const std::string& extensionName);
    virtual void UnloadSystemExtension(const ISystemExtensionPtr& systemExtension);

    inline void SetSharedLoaderFactory(const ISharedLibraryLoaderFactoryPtr& proxyFactory);

  private:
    logging::GloveLogger logger;

    SystemExtensionMap loadedExtensions;

    ISharedLibraryLoaderFactoryPtr proxyFactory;

    ISharedLibraryLoaderPtr CreateLibraryLoader(const std::string& libraryFilePath) {
        return proxyFactory->CreateLibraryLoader(libraryFilePath);
    }
};
} /* namespace glove */