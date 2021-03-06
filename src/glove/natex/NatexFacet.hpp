#pragma once

#include <list>

#include "glove/BlueDwarfApi.hpp"
#include "glove/application/DwarfApplication.hpp"
#include "glove/services/ServiceRegistry.hpp"
#include "glove/natex/ExtensionSearcher.hpp"
#include "glove/natex/ModuleLoader.hpp"

namespace BlueDwarf {
class BD_API_EXPORT NatexFacet : public ApplicationFacet {
  public:
    NatexFacet(ServiceRegistryHandle serviceRegistry);
    NatexFacet(ServiceRegistryHandle serviceRegistry, std::shared_ptr<ModuleLoader> moduleLoader);

    void AddExtensionSearcher(std::shared_ptr<ExtensionSearcher> searcher);

    virtual void Bootstrap() override;

  private:
    void LoadNativeModules(ExtensionSearcher& searcher);

    bool loadNativeExtensions = true;

    ServiceRegistryHandle serviceRegistry;
    std::shared_ptr<ModuleLoader> moduleLoader;
    std::list<std::shared_ptr<ExtensionSearcher>> extensionSearchers;

    std::list<std::shared_ptr<Module>> modules;

};
}