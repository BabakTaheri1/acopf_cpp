#pragma once
#include <string>
#include "utils/Types.hpp"

namespace acopf {

// Loads a MATPOWER case saved as a .mat file.
PowerGrid LoadMatpowerCase(const std::string& filename);

} // namespace acopf
