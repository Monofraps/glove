#include "GlovePlugin.h"

#include <boost/format.hpp>

#include "core/GloveException.h"
#include "core/GloveCore.h"
#include "scripting/GlovePythonEngine.h"
#include "scripting/PythonPlugin.h"

namespace glove {

GlovePlugin::GlovePlugin(std::string name) : name(name), EnableProfilable() {
    pluginBasePath = gloveCore->MakeDataPath((boost::format("data/game/plugins/%1%") % name).str());
    scriptsPath = (boost::format("%1%/scripts") % pluginBasePath).str();

    try {
        pythonPlugin = std::make_shared<PythonPlugin>(pluginBasePath, name);
    }
    catch (const boost::python::error_already_set&) {
        gloveCore->GetPythonEngineRef()->HandleError();

        throw GLOVE_EXCEPTION((boost::format("Failed to create initialize plugin %1% because the python plugin couldn't be imported.") % name).str());
    }
}

GlovePlugin::~GlovePlugin() {

}

void GlovePlugin::LoadPlugin() {
    pythonPlugin->LoadPlugin();
}

void GlovePlugin::ReloadPlugin() {
    pythonPlugin->ReloadPlugin();
}

void GlovePlugin::UnloadPlugin() {
    pythonPlugin->UnloadPlugin();
}

bool GlovePlugin::IsLoaded() const {
    return pythonPlugin->IsLoaded();

}
} // namespace glove