cmake_minimum_required (VERSION 2.8)
project(BRUKER-ISMRMRD)

#VERSIONING
set(BRUKER_TO_ISMRMRD_VERSION_MAJOR 0)
set(BRUKER_TO_ISMRMRD_VERSION_MINOR 5)
set(BRUKER_TO_ISMRMRD_VERSION_PATCH 0)
set(BRUKER_TO_ISMRMRD_VERSION_STRING ${BRUKER_TO_ISMRMRD_VERSION_MAJOR}.${BRUKER_TO_ISMRMRD_VERSION_MINOR}.${BRUKER_TO_ISMRMRD_VERSION_PATCH})
enable_testing()

#Set the build type to Release if not specified
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif (NOT CMAKE_BUILD_TYPE)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# build the tools
add_subdirectory(src)
