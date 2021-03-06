#include <fstream>

#include <boost/format.hpp>

#include "glove/filesystem/internal/UnixNativeFilesystem.hpp"
#include "glove/DwarfException.hpp"
#include "glove/utils/RuntimePathInfo.hpp"
#include "glove/filesystem/exceptions/FileInfoQueryException.hpp"

namespace BlueDwarf {
UnixNativeFilesystem::UnixNativeFilesystem() {
    std::string executableName;

    GetExecutionPathInfo(this->nativeRoot, executableName);
}

UnixNativeFilesystem::UnixNativeFilesystem(const std::string& nativeRoot) {
    if(nativeRoot.back() == '/') {
        this->nativeRoot = nativeRoot.substr(0, nativeRoot.length() - 2);
    }
}

const FileInfo UnixNativeFilesystem::GetFileInfo(const Path& path) {
    struct stat unixFileInfo;
    if(stat(MakePathNativeAbsolute(path).ToString().c_str(), &unixFileInfo) == -1) {
        switch (errno) {
            case EFAULT:
                DWARF_THROW(FileInfoQueryException, path, "EFAULT - `path` pointed to illegal address");

            case ELOOP:
                DWARF_THROW(FileInfoQueryException, path, "ELOOP - A loop exists in symbolic links encountered during path resolution");

            case ENAMETOOLONG:
                DWARF_THROW(FileInfoQueryException, path, "ENAMETOOLONG - The path's length exceeds PATH_MAX");

            case ENOENT:
                return FileInfo(FileInfo::FileType::NOT_FOUND, 0);

            case ENOMEM:
                DWARF_THROW(FileInfoQueryException, path, "ENOMEM - Out of kernel memory");

            case ENOTDIR:
                return FileInfo(FileInfo::FileType::NOT_FOUND, 0);

            case EOVERFLOW:
                DWARF_THROW(FileInfoQueryException, path, "EOVERFLOW - The file's size is too big to represent");

            default:
                DWARF_THROW(FileInfoQueryException, path, (boost::format("Unknown error - %1%") % strerror(errno)).str());
        }
    }

    if(S_ISREG(unixFileInfo.st_mode)) {
        return FileInfo(FileInfo::FileType::REGULAR, static_cast<std::size_t>(unixFileInfo.st_size));
    } else if(S_ISDIR(unixFileInfo.st_mode)) {
        return FileInfo(FileInfo::FileType::DIRECTORY, static_cast<std::size_t>(unixFileInfo.st_size));
    } else {
        return FileInfo(FileInfo::FileType::UNKNOWN, static_cast<std::size_t>(unixFileInfo.st_size));
    }
}

std::istream* UnixNativeFilesystem::CreateFileReadStream(const Path& path) {
    return new std::ifstream(path.ToString(), std::ios::in | std::ios::binary);
}

std::ostream* UnixNativeFilesystem::CreateFileWriteStream(const Path& path) {
    return new std::ofstream(path.ToString(), std::ios::out | std::ios::binary);
}

Path UnixNativeFilesystem::MakePathNativeAbsolute(const Path& path) {
    if(path.IsAbsolute()) {
        return Path(nativeRoot + path.ToString());
    }

    return Path(nativeRoot + "/" + path.ToString());
}
}