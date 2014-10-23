#include "IndexData.h"

#include "core/GloveCore.h"
#include "core/GpuBufferManager.h"

namespace glove {

IndexData::IndexData() : EnableProfilable() {
    indexBuffer = gloveCore->GetGpuBufferManager()->CreateIndexBuffer();
}

IndexData::~IndexData() {

}

} // namespace glove