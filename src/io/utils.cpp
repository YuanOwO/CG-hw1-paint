#include "io/utils.hpp"

#include <stdexcept>

namespace paint::io::utils {

void validateWritePath(const fs::path& path) {
    if (path.empty()) {
        throw std::runtime_error("Cannot write: file path is empty");
    }

    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            throw std::runtime_error("Cannot write to a directory: " + path.string());
        }

        if (!fs::is_regular_file(path)) {
            throw std::runtime_error("Cannot write to a non-regular file: " + path.string());
        }
    }
}

void validateReadPath(const fs::path& path) {
    if (path.empty()) {
        throw std::runtime_error("Cannot read: file path is empty");
    }

    if (!fs::exists(path)) {
        throw std::runtime_error("Cannot read: file does not exist: " + path.string());
    }

    if (fs::is_directory(path)) {
        throw std::runtime_error("Cannot read from a directory: " + path.string());
    }

    if (!fs::is_regular_file(path)) {
        throw std::runtime_error("Cannot read from a non-regular file: " + path.string());
    }
}

}  // namespace paint::io::utils
