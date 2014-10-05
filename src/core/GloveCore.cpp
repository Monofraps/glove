/*
 * GloveCore.cpp
 *
 *  Created on: Aug 18, 2014
 *      Author: monofraps
 */

#include "GloveCore.h"

#include <chrono>

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#elif defined(__unix__) || defined(__unix)
#include <unistd.h>
#include <cstdlib>
#endif

#include <boost/format.hpp>
#include <GLFW/glfw3.h>

#include "GloveException.h"
#include "graph/Scenegraph.h"
#include "graph/GameObject.h"
#include "graph/GameComponent.h"
#include "rendering/GloveWindow.h"
#include "rendering/GloveRenderer.h"
#include "core/GpuBufferManager.h"
#include "core/PluginLoader.h"
#include "scripting/GlovePythonEngine.h"
#include "shader/pyshed/PyShedLoader.h"
#include "event/EventBus.h"
#include "input/InputManager.h"

namespace sc = std::chrono;

namespace glove {

GloveCorePointer GloveCore::instance;

GloveCore::GloveCore() 
	:	GloveObject(false), 
		frameCounter(0),
		exitRequested(false)
{
	frameData.frameId = 0;
	frameData.viewProjectionMatrix = glm::mat4();
	frameData.deltaTime = 0;
	
#if defined(_WIN32) || defined(WIN32)
	int bufferSize = 4096;
	char* moduleName = reinterpret_cast<char*>(GloveMemAllocN(sizeof(char) * bufferSize, "GloveCore/ModuleName"));
	GetModuleFileName(NULL, moduleName, bufferSize);

	while (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		GloveMemFree(moduleName);
		bufferSize += 64;
		moduleName = reinterpret_cast<char*>(GloveMemAllocN(sizeof(wchar_t) * bufferSize, "GloveCore/ModuleName"));

		GetModuleFileName(NULL, moduleName, bufferSize);
	}

	executableName = moduleName;
	GloveMemFree(moduleName);
#elif defined(__unix__) || defined(__unix)
	int bufferSize = 1024;
	char* linkName = reinterpret_cast<char*>(GloveMemAllocN(bufferSize, "GloveCore/SelfLinkName"));
	ssize_t charsWritten = readlink("/proc/self/exe", linkName, bufferSize);

	while(charsWritten == bufferSize) {
		GloveMemFree(linkName);
		bufferSize += 64;
		linkName = reinterpret_cast<char*>(GloveMemAllocN(bufferSize, "GloveCore/SlefLinkName"));
		charsWritten = readlink("/proc/self/exe", linkName, bufferSize);
	}

	OLOG(info, charsWritten);
	linkName[charsWritten] = '\0';

	executableName = linkName;
	GloveMemFree(linkName);
#endif
	executablePath = executableName.substr(0, executableName.find_last_of("\\/"));

	OLOG(info, "Running from " << executablePath);
	OLOG(info, "GloveCore created");
}

GloveCore::~GloveCore() {
}

void GloveCore::Init(int argc, char** argv) {
	try {
		initializationTime = sc::steady_clock::now();

		instance = GloveCorePointer(this);

		eventBus = EventBusPtr(new EventBus());

		InitializeRenderingSystem(argc, argv, 800, 600);
		primaryScenegraph = ScenegraphPtr(new Scenegraph());

		inputManager = InputManagerPtr(new InputManager());
		eventBus->Subscribe(inputManager);

		InitializeScripting();
		InitializeResourceLoaders();

		TimePoint initializationDone = sc::steady_clock::now();
		auto timeSpan = sc::duration_cast<std::chrono::milliseconds>(initializationDone - initializationTime);
		OLOG(info, "Engine initialization took " << timeSpan.count() << "ms");
	}
	catch (const GloveException& ex) {
		OLOG(error, (boost::format("Engine initialization failed: %1%") % ex.what()).str());
	}

}

void GloveCore::InitializeRenderingSystem(int argc, char** argv, int windowWidth, int windowHeight) {
	renderer = GloveRendererPtr(new GloveRenderer());

	try {
		renderer->Init(windowWidth, windowHeight, 3, 3, argc, argv);
	}
	catch (const GloveException& e) {
		OLOG(error, "Exception while initializing rendering subsystem:" << std::endl << e.what());
	}

	gpuBufferManager = GpuBufferManagerPtr(new GpuBufferManager());	
}

void GloveCore::InitializeScripting() {
	pythonEngine = GlovePythonEnginePtr(new GlovePythonEngine(executablePath));

	try {
		namespace bpy = boost::python;
		bpy::object mainNs = pythonEngine->GetBuiltins();

		mainNs["g_Core"] = bpy::object(shared_from_this());
		mainNs["g_Scenegraph"] = bpy::object(primaryScenegraph);
	}
	catch (boost::python::error_already_set const &) {
		pythonEngine->HandleError();

		throw GLOVE_EXCEPTION("Failed to expose global scripting objects");
	}
}

void GloveCore::InitializeResourceLoaders() {
	pyshedLoader = PyShedLoaderPtr(new PyShedLoader(pythonEngine));
	pluginLoader = PluginLoaderPtr(new PluginLoader());

	pluginLoader->DiscoverPlugins();
	pluginLoader->LoadPlugins();
}

void GloveCore::EnterMainLoop() {
	TimePoint start = std::chrono::steady_clock::now();
	TimePoint end = std::chrono::steady_clock::now();
	
	while (!IsExitRequested()) {
		end = std::chrono::steady_clock::now();
		lastFrameTime = sc::duration_cast<std::chrono::duration<double>>(end - start);
		start = std::chrono::steady_clock::now();

		frameData.frameId++;
		frameData.deltaTime = lastFrameTime.count();

		Update();
		Render(primaryScenegraph);
	}
}

void GloveCore::Update() {
	glfwPollEvents();
	
	if (inputManager->IsKeyPressed(KC_F5)) {
		OLOG(info, memory_internal::DumpList());
	}

	if (inputManager->IsKeyPressed(KC_F6)) {
		OLOG(info, (boost::format("Last Update Time: %1%ms") % std::chrono::duration_cast<std::chrono::milliseconds>(lastFrameTime).count()).str());
	}

	primaryScenegraph->IterateGameObjects([&](GameObjectPtr gameObject){
		gameObject->IterateComponents([&](const GameComponentPtr& gameComponent){
			gameComponent->SyncEarlyUpdate();
		});

		gameObject->IterateComponents([&](const GameComponentPtr& gameComponent){
			gameComponent->SyncUpdate();
		});

		gameObject->IterateComponents([&](const GameComponentPtr& gameComponent){
			gameComponent->AsyncUpdate();
		});

		gameObject->IterateComponents([&](const GameComponentPtr& gameComponent){
			gameComponent->SyncLateUpdate();
		});
	});
	
	inputManager->SyncUpdate();	
}

void GloveCore::Render(ScenegraphPointer scenegraph) {
	renderer->RenderScene(scenegraph, frameData);
}

std::string GloveCore::MakeDataPath(const std::string& relPath) const {
	return (boost::format("%1%/%2%") % executablePath % relPath).str();
}

} /* namespace glove */
