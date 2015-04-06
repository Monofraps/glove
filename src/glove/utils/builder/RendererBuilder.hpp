#pragma once

#include <string>

#include "glove/GloveApi.hpp"
#include "glove/GloveFwd.hpp"
#include "glove/rendering/Rendering.hpp"
#include "glove/configuration/Configuration.hpp"
#include "glove/utils/ScreenDimensions.hpp"
#include "glove/services/Services.hpp"

namespace glove {

class GLOVE_API_EXPORT RendererBuilder {
  public:
    RendererBuilder(const ServiceHandle& rendererFactory);
    RendererBuilder(const IRendererFactoryPtr& rendererFactory);
    RendererBuilder(const ServiceHandle& rendererFactory, const Configuration& configuration);
    RendererBuilder(const IRendererFactoryPtr& rendererFactory, const Configuration& configuration);
    RendererBuilder(const GloveCorePtr& core);

    RendererBuilder& SetWindowTitle(const std::string& title);
    RendererBuilder& SetWindowSize(const ScreenDimensions& size);
    RendererBuilder& SetWindowSize(int width, int height);
    RendererBuilder& SetWindowPosition(const ScreenPoint& screenPoint);
    RendererBuilder& SetWindowPosition(int x, int y);

    IRendererPtr Finalize();

  private:
    std::string windowTitle;
    ScreenDimensions windowSize;
    ScreenPoint windowPosition;
    IRendererFactoryPtr rendererFactory;
};

} /* namespace glove */