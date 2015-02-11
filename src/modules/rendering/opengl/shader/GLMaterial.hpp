#pragma once

#include <string>
#include <map>

#include <glove/rendering/Rendering.hpp>
#include <glove/rendering/shader/IMaterial.hpp>
#include <glove/log/Log.hpp>

#include "subsystem/OpenGLRendererModule.hpp"

namespace glove {
namespace gl {

/// @ingroup Shader
class GLMaterial : public IMaterial, public std::enable_shared_from_this<GLMaterial> {
  public:
    GLMaterial(const GLShaderProgramPtr& shader);
    virtual ~GLMaterial() = default;

    virtual IShaderProgramPtr GetShaderProgram() const;
    virtual IMaterialAttributePtr GetMaterialAttribute(const std::string& name);
    virtual IMaterialAttributePtr GetMaterialAttribute(const MaterialAttributeSemantic semantic);
    virtual void MapAttributeSemantic(const MaterialAttributeSemantic semantic, const std::string& attributeName);

private:
    logging::GloveLogger logger;

    GLShaderProgramPtr shader;

    std::map<MaterialAttributeSemantic, IMaterialAttributePtr> attributeSemanticMap;
    std::map<std::size_t, IMaterialAttributePtr> attributeNameMap;
};
}
} // namespace glove