project(graph-coursework)
cmake_minimum_required(VERSION 2.8)

add_definitions(-std=c++11 -Wall)

# Build type
set(CMAKE_BUILD_TYPE Debug)

# Debug levels
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUG_LEVEL=4 -O0)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
  add_definitions(-DDEBUG_LEVEL=1 -O2)
endif()

# Various packages
find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})
find_package(SDL_ttf REQUIRED)
include_directories(${SDLTTF_INCLUDE_DIR})
find_package(Boost REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIR})

link_libraries(${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY} ${Boost_LIBRARIES})

include_directories(src)
aux_source_directory(src SRC_LIST)
aux_source_directory(src/sdlobj SDLOBJ_LIST)
aux_source_directory(src/common COMMON_LIST)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SDLOBJ_LIST} ${COMMON_LIST})
