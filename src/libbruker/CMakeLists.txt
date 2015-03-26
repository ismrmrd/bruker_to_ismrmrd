find_package(FLEX REQUIRED)

FLEX_TARGET(BrukerScanner brukerlex.l ${CMAKE_CURRENT_BINARY_DIR}/brukerlex.cpp)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_definitions(-fPIC)
add_library(
    bruker
    SHARED
    brukerparameterparser.cpp
    brukerrawdata.cpp
    ndarray.cpp
    ${FLEX_BrukerScanner_OUTPUTS}
)

install(TARGETS bruker DESTINATION lib)

