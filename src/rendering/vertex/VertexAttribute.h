#pragma once

#include <cstddef>

#include <pitamem/MemoryProfile.h>
#include "rendering/vertex/VertexAttributeSemantic.h"
#include "rendering/vertex/VertexAttributeType.h"

namespace glove {

/** Represents a vertex attribute */
class VertexAttribute {
Profilable();
public:
    /**
    * @param [in] binding The buffer binding this attribute belongs to (relates to the buffer binding slot in VertexData)
    * @param [in] offset The attribute's offset in bytes
    * @param [in] attributeType The attribute's type
    * @param [in] attributeSemantic The attribute's semantic
    */
	VertexAttribute(std::size_t binding, std::size_t offset, VertexAttributeType attributeType, VertexAttributeSemantic attributeSemantic);

    virtual ~VertexAttribute();

    /** Returns the binding slot */
    std::size_t GetBindingSlot() const {
        return binding;
    }

    /** Returns the attribute offset */
    std::size_t GetOffset() const {
        return offset;
    }

    /** Returns the attribute type */
    VertexAttributeType GetType() const {
        return attributeType;
    }

    /** Returns the attribute semantic */
    VertexAttributeSemantic GetSemantic() const {
        return attributeSemantic;
    }

    /** Returns the number of individual components - e.g. FLOAT3 has 3 float components */
    std::size_t GetNumberOfComponents() const;

    /** Returns the total size of @p attributeType in bytes */
    static std::size_t GetSize(VertexAttributeType attributeType);

    /** Returns the total size of this attribute in bytes */
    std::size_t GetSize() const;

protected:
    size_t binding;
    size_t offset;
    VertexAttributeType attributeType;
    VertexAttributeSemantic attributeSemantic;
};


} // namespace glove