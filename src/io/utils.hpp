#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace paint::io::utils {

void validateWritePath(const fs::path& path);
void validateReadPath(const fs::path& path);

}  // namespace paint::io::utils
