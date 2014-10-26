#include "GlovePythonEngine.h"

#include <stdio.h>

#include <boost/format.hpp>
#include <boost/python.hpp>
#include <boost/filesystem.hpp>

#include "core/GloveException.h"
#include "scripting/PythonPlugin.h"
#include "modules/glove/GloveModule.h"
#include "modules/pyshed/PyShedModule.h"

namespace bpy = boost::python;
namespace bfs = boost::filesystem;

namespace glove {

GlovePythonEngine::GlovePythonEngine(const std::string& executableBasePath)
        : basePath(executableBasePath), EnableProfilable() {
#if defined(WIN32)
    const wchar_t pathSeparator = L';';
    #else
    const wchar_t pathSeparator = L':';
#endif

    std::wstringstream wstringConverter;
    wstringConverter << executableBasePath.c_str();
    std::wstring wideBasePath = wstringConverter.str();

    std::wstring pythonHome = (boost::wformat(L"%1%/data/python%2%%1%/data/game/modules%2%%1%/data/game/plugins") % wideBasePath % pathSeparator).str();
    wchar_t* cstrPythonHome = new wchar_t[pythonHome.length()];
    wcscpy(cstrPythonHome, pythonHome.c_str());

    try {
        Py_SetPath(cstrPythonHome);
        LOG(logger, info, "Using Python in " << std::wstring(Py_GetPath()));

        PyImport_AppendInittab("glove", &python::CreateGloveModule);
        PyImport_AppendInittab("pyshed", &python::CreatePyshedModule);
        Py_Initialize();

        using namespace boost::python;

        // Init root namespace
        dict builtinDict = GetBuiltins();
        rootNamespace["__builtins__"] = builtinDict;
        rootNamespace["__plugins__"] = pluginScopes;
        rootNamespace["sys"] = import("sys");

        // Add glove module types to builtins
        dict gloveModuleDict = extract<dict>(import("glove").attr("__dict__"));
        builtinDict.update(gloveModuleDict);

        LoadPyEnvironmentModule();
    }
    catch (const boost::python::error_already_set&) {
        HandleError();

        throw GLOVE_EXCEPTION("Failed to create scripting engine.");
    }

    LOG(logger, info, "Scripting engine initialized.");
}

GlovePythonEngine::~GlovePythonEngine() {
    // Unload all plugins
    for (PythonPluginPtr plugin : plugins) {
        if (plugin->IsLoaded()) {
            plugin->UnloadPlugin();
        }
    }

    Py_Finalize();
}

void GlovePythonEngine::LoadPyEnvironmentModule() {
    std::string gloveCorePythonEnvironmentDirBase = basePath;
    gloveCorePythonEnvironmentDirBase.append("/data/game/gcpyenv");

    bfs::path gloveCorePythonEnvDir(gloveCorePythonEnvironmentDirBase);

    bfs::directory_iterator endIter;
    for (bfs::directory_iterator dir = bfs::directory_iterator(gloveCorePythonEnvDir); dir != endIter; dir++) {
        if (bfs::is_regular_file(*dir)) {
            LOG(logger, info, "Loading python environment file: " << dir->path().filename().string());

            std::string path = dir->path().string();
            bpy::exec_file(path.c_str(), rootNamespace);
        }
    }

    LOG(logger, info, "PyEnv files loaded");
}

void GlovePythonEngine::AnnouncePlugin(const PythonPluginPtr& pythonPlugin) {
    if (pluginScopes.contains(pythonPlugin->GetPluginName().c_str())) {
        LOG(logger, warning, (boost::format("Plugin %1% has already been announced. Overwriting previous dict") % pythonPlugin->GetPluginName()).str());
    }

    pluginScopes[pythonPlugin->GetPluginName().c_str()] = pythonPlugin->GetScope();
}

void GlovePythonEngine::HandleError() {
    PyObject* pyType;
    PyObject* pyValue;
    PyObject* pyTraceback;

    PyErr_Fetch(&pyType, &pyValue, &pyTraceback);
    PyErr_NormalizeException(&pyType, &pyValue, &pyTraceback);
    if (pyType == NULL && pyValue == NULL && pyTraceback == NULL) {
        LOG(logger, warning, "GPythonEngine::HandleError() called but no error was set.");
    }

    using namespace boost::python;
    handle<> hType(pyType);
    handle<> hValue(allow_null(pyValue));
    handle<> hTraceback(allow_null(pyTraceback));

    std::string error;
    if (hValue) {
        object traceback(import("traceback"));
        object formatException(traceback.attr("format_exception"));
        object formattedList(formatException(hType, hValue, hTraceback));
        object formatted(str("").join(formattedList));

        error = extract<std::string>(formatted);
    }
    else {
        error = extract<std::string>(str(hType));
    }

    LOG(logger, info, "Python Error" << std::endl << error);
}

bpy::object GlovePythonEngine::GetMainModule() {
    return bpy::import("__main__");
}

bpy::dict GlovePythonEngine::GetRootNamespace() {
    return rootNamespace;
}

bpy::dict GlovePythonEngine::GetBuiltins() {
    return bpy::extract<bpy::dict>(GetMainModule().attr("__dict__")["__builtins__"].attr("__dict__"));
}

} //namespace glove
