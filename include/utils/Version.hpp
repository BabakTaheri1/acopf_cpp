#pragma once
#include <string>

namespace acopf {

struct Version {
  static int Major();
  static int Minor();
  static int Patch();

  static std::string String();        
  static std::string GitDescribe();  
};

} // namespace acopf
