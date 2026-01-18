cmake_minimum_required(VERSION 3.16)
project(acopf CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


option(BUILD_MODULAR "Build the folder-based modular code" ON)
option(BUILD_LEGACY  "Build the single-file legacy acopf.cpp" ON)

option(ENABLE_IPOPT  "Enable IPOPT targets" ON)
option(ENABLE_KNITRO "Enable KNITRO targets" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

add_compile_options($<$<CONFIG:Release>:-O3>)
add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)


option(ENABLE_NATIVE_ARCH "Use -march=native (like your Makefile)" ON)
if(ENABLE_NATIVE_ARCH)
  add_compile_options(-march=native)
endif()




find_path(MATIO_INCLUDE_DIR matio.h
  HINTS /opt/homebrew/include
)
find_library(MATIO_LIBRARY matio
  HINTS /opt/homebrew/lib
)
if(NOT MATIO_INCLUDE_DIR OR NOT MATIO_LIBRARY)
  message(FATAL_ERROR "Could not find matio. Set MATIO_INCLUDE_DIR / MATIO_LIBRARY.")
endif()

# IPOPT (only needed if ENABLE_IPOPT)
# On many mac setups it lives under homebrew; we mimic your includes.
if(ENABLE_IPOPT)
  find_path(IPOPT_INCLUDE_DIR IpTNLP.hpp
    HINTS /opt/homebrew/include /opt/homebrew/include/coin-or
  )
  find_library(IPOPT_LIBRARY ipopt
    HINTS /opt/homebrew/lib
  )
  if(NOT IPOPT_INCLUDE_DIR OR NOT IPOPT_LIBRARY)
    message(FATAL_ERROR "ENABLE_IPOPT=ON but Ipopt headers/libs not found.")
  endif()
endif()

# Knitro (only needed if ENABLE_KNITRO)
set(KNITRODIR "" CACHE PATH "Path to Knitro root directory (contains include/ and lib/)")
if(ENABLE_KNITRO)
  if(NOT KNITRODIR)
    message(FATAL_ERROR "ENABLE_KNITRO=ON but KNITRODIR is empty. Pass -DKNITRODIR=...")
  endif()

  set(KNITRO_INCLUDE_DIR "${KNITRODIR}/include")
  set(KNITRO_LIB_DIR     "${KNITRODIR}/lib")

  if(NOT EXISTS "${KNITRO_INCLUDE_DIR}/knitro.h")
    message(FATAL_ERROR "knitro.h not found at ${KNITRO_INCLUDE_DIR}. Check KNITRODIR.")
  endif()

  # Your Makefile linked -lknitro1510; keep that name.
  find_library(KNITRO_LIBRARY knitro1510 HINTS "${KNITRO_LIB_DIR}")
  if(NOT KNITRO_LIBRARY)
    message(FATAL_ERROR "Could not find knitro library in ${KNITRO_LIB_DIR}.")
  endif()
endif()



set(HOMEBREW_INC /opt/homebrew/include)
set(HOMEBREW_COINOR_INC /opt/homebrew/include/coin-or)
set(HOMEBREW_LIB /opt/homebrew/lib)


if(BUILD_MODULAR)
  add_library(acopf_modular_lib
    src/MatpowerLoader.cpp
    src/utils/Version.cpp
    src/AcopfModel.cpp
    src/Solve.cpp
    src/utils/Fs.cpp
    src/utils/Summary.cpp
    src/utils/Timer.cpp
  )

  target_include_directories(acopf_modular_lib
    PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/include
      ${MATIO_INCLUDE_DIR}
      ${HOMEBREW_INC}
  )

  target_link_libraries(acopf_modular_lib
    PUBLIC
      ${MATIO_LIBRARY}
  )

  if(ENABLE_IPOPT)
    add_library(acopf_ipopt_impl src/IpoptSolver.cpp)
    target_include_directories(acopf_ipopt_impl
      PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${IPOPT_INCLUDE_DIR}
        ${HOMEBREW_INC}
        ${HOMEBREW_COINOR_INC}
    )
    target_link_libraries(acopf_ipopt_impl PUBLIC ${IPOPT_LIBRARY})

    add_executable(acopf_mod_ipopt main.cpp)
    target_link_libraries(acopf_mod_ipopt PRIVATE acopf_modular_lib acopf_ipopt_impl)
    target_compile_definitions(acopf_mod_ipopt PRIVATE) # no USE_KNITRO
  endif()

  # KNITRO modular executable
  if(ENABLE_KNITRO)
    add_library(acopf_knitro_impl src/KnitroSolver.cpp)
    target_compile_definitions(acopf_knitro_impl PUBLIC USE_KNITRO)
    target_include_directories(acopf_knitro_impl
      PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${KNITRO_INCLUDE_DIR}
        ${HOMEBREW_INC}
    )
    target_link_libraries(acopf_knitro_impl PUBLIC ${KNITRO_LIBRARY})

    add_executable(acopf_mod_knitro main.cpp)
    target_compile_definitions(acopf_mod_knitro PRIVATE USE_KNITRO)
    target_link_libraries(acopf_mod_knitro PRIVATE acopf_modular_lib acopf_knitro_impl)

    set_target_properties(acopf_mod_knitro PROPERTIES
      BUILD_RPATH "${KNITRO_LIB_DIR}"
    )
  endif()
endif()



if(BUILD_LEGACY)
  # Legacy IPOPT
  if(ENABLE_IPOPT)
    add_executable(acopf_legacy_ipopt acopf.cpp)
    target_include_directories(acopf_legacy_ipopt PRIVATE
      ${MATIO_INCLUDE_DIR}
      ${IPOPT_INCLUDE_DIR}
      ${HOMEBREW_INC}
      ${HOMEBREW_COINOR_INC}
    )
    target_link_libraries(acopf_legacy_ipopt PRIVATE
      ${MATIO_LIBRARY}
      ${IPOPT_LIBRARY}
    )
  endif()

  # Legacy KNITRO
  if(ENABLE_KNITRO)
    add_executable(acopf_legacy_knitro acopf.cpp)
    target_compile_definitions(acopf_legacy_knitro PRIVATE USE_KNITRO)
    target_include_directories(acopf_legacy_knitro PRIVATE
      ${MATIO_INCLUDE_DIR}
      ${KNITRO_INCLUDE_DIR}
      ${HOMEBREW_INC}
    )
    target_link_libraries(acopf_legacy_knitro PRIVATE
      ${MATIO_LIBRARY}
      ${KNITRO_LIBRARY}
    )
    set_target_properties(acopf_legacy_knitro PROPERTIES
      BUILD_RPATH "${KNITRO_LIB_DIR}"
    )
  endif()
endif()


message(STATUS "BUILD_MODULAR=${BUILD_MODULAR} BUILD_LEGACY=${BUILD_LEGACY}")
message(STATUS "ENABLE_IPOPT=${ENABLE_IPOPT} ENABLE_KNITRO=${ENABLE_KNITRO}")
message(STATUS "MATIO_INCLUDE_DIR=${MATIO_INCLUDE_DIR}")
message(STATUS "MATIO_LIBRARY=${MATIO_LIBRARY}")
if(ENABLE_KNITRO)
  message(STATUS "KNITRODIR=${KNITRODIR}")
endif()
