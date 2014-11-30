#if defined(ON_UNIX)

#include "SoLoaderFactory.h"

#include "SoLoader.h"

namespace glove {
ISharedLibraryLoaderPtr glove::SoLoaderFactory::CreateLibraryLoader(const std::string& libraryFilePath) {
    return std::make_shared<SoLoader>(libraryFilePath);
}
}

#endif
