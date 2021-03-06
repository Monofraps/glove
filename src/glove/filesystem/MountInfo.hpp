#pragma once

#include "glove/BlueDwarfApi.hpp"
#include "glove/filesystem/Filesystem.hpp"
#include "glove/filesystem/Path.hpp"

namespace BlueDwarf {

class BD_API_EXPORT MountInfo {
  public:
    MountInfo(FilesystemHandle const& filesystem, Path const& mountLocation)
            : filesystem(filesystem), mountLocation(mountLocation) {}


    const FilesystemHandle& GetFilesystem() const {
        return filesystem;
    }

    const Path& GetMountLocation() const {
        return mountLocation;
    }

  private:
    const FilesystemHandle filesystem;
    const Path mountLocation;
};

} /* namespace BlueDwarf */