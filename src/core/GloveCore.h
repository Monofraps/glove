/*
 * GloveCore.h
 *
 *  Created on: Aug 18, 2014
 *      Author: monofraps
 */

#ifndef GLOVECORE_H_
#define GLOVECORE_H_

#include <chrono>

#include "GloveFwd.h"

#include "rendering/GloveRenderer.h"
#include "GloveObject.h"
#include "GloveException.h"
#include "rendering/FrameData.h"

namespace glove {

class GloveCore : public GloveObject, public std::enable_shared_from_this<GloveCore> {
	GLOVE_MEM_ALLOC_FUNCS("GloveCore")
public:
	GloveCore();
	virtual ~GloveCore();

	/** Initializes the Glove engine and all subsystems. Must be called from main thread. */
	void Init(int argc, char** argv);
		
	/** Starts the main application loop. Must be called from main thread. */
	void EnterMainLoop();

	void Update();
	void Render(ScenegraphPointer scenegraph);

	GlovePythonEnginePtr GetPythonEngine() const { return pythonEngine; }
	PyShedLoaderPtr GetPyshedLoader() const { return pyshedLoader; }
	GloveRendererPointer GetRenderer() const { return renderer; }
	ScenegraphPtr GetScenegraph() const { return primaryScenegraph; }
	GpuBufferManagerPtr GetGpuBufferManager() const { return gpuBufferManager; }
	PluginLoaderPtr GetPluginLoader() const { return pluginLoader; }
	EventBusPtr GetEventBus() const { return eventBus; }
	InputManagerPtr GetInputManager() const { return inputManager; }

	const GlovePythonEnginePtr& GetPythonEngineRef() const { return pythonEngine; }
	const PyShedLoaderPtr& GetPyshedLoaderRef() const { return pyshedLoader; }
	const GloveRendererPointer& GetRendererRef() const { return renderer; }
	const ScenegraphPtr& GetScenegraphRef() const { return primaryScenegraph; }
	const GpuBufferManagerPtr& GetGpuBufferManagerRef() const { return gpuBufferManager; }
    const PluginLoaderPtr& GetPluginLoaderRef() const { return pluginLoader; }
    const EventBusPtr& GetEventBusRef() const { return eventBus; }
	const InputManagerPtr& GetInputManagerRef() const{ return inputManager; }
	
	std::string MakeDataPath(const std::string& relPath) const;
	
	void OnKeyEvent(const KeyEvent& evnt);

	bool IsExitRequested() const { return exitRequested; }
	void SetExitRequested(bool requestExit) { exitRequested = requestExit; }

	static inline GloveCorePointer Instance() {
		if (GloveCore::instance) {
			return GloveCore::instance;
		}
		else {
			throw new GloveException("GloveCore has not been initialized");
		}
	}

private:
	typedef std::chrono::steady_clock::time_point TimePoint;

	EventBusPtr eventBus;
	GloveRendererPtr renderer;
	GlovePythonEnginePtr pythonEngine;
	PyShedLoaderPtr pyshedLoader;
	GpuBufferManagerPtr gpuBufferManager;
	ScenegraphPtr primaryScenegraph;
	PluginLoaderPtr pluginLoader;
	InputManagerPtr inputManager;

	TimePoint initializationTime;

	std::chrono::duration<double> lastFrameTime;

	FrameData frameData;

	std::string executableName;
	std::string executablePath;

	static GloveCorePointer instance;

	unsigned long frameCounter;
	bool exitRequested;
	
	void InitializeRenderingSystem(int argc, char** argv, int windowWidth, int windowHeight);
	void InitializeScripting();
	void InitializeResourceLoaders();
};

} /* namespace glove */

#endif /* GLOVECORE_H_ */
