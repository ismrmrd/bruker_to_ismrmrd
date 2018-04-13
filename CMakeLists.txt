cmake_minimum_required (VERSION 2.8)
project(BRUKER-ISMRMRD)

enable_testing()

#Set the build type to Release if not specified
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif (NOT CMAKE_BUILD_TYPE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if(CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_STANDARD 11)
endif()

# build the tools
add_subdirectory(src)
