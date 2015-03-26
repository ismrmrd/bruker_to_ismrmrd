# Build the bruker file io library
add_subdirectory(libbruker)

# Build the converter
find_package(Ismrmrd REQUIRED)

set(Boost_NO_BOOST_CMAKE ON)

if(WIN32)
  find_package(Boost COMPONENTS program_options thread system date_time chrono REQUIRED)
else(WIN32)
  find_package(Boost COMPONENTS program_options thread system REQUIRED)
endif(WIN32)

if(WIN32)
  link_directories(${Boost_LIBRARY_DIRS})
endif(WIN32)

include_directories(
  ${Boost_INCLUDE_DIR}	
  ${ISMRMRD_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/libbruker
  )

add_executable(bruker_to_ismrmrd main.cpp)
target_link_libraries(bruker_to_ismrmrd bruker ${ISMRMRD_LIBRARIES} ${Boost_LIBRARIES})

install(TARGETS bruker_to_ismrmrd DESTINATION bin COMPONENT main)


