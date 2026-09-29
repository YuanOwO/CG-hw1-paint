#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace paint::utils {

void validateWritePath(const fs::path& path);

}  // namespace paint::utils
