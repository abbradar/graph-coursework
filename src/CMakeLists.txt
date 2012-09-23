project(graph-coursework)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall")

# Build type
set(CMAKE_BUILD_TYPE Debug)

# Debug levels
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUG_LEVEL=4)
elseif(CMAKE_BUILD_TYPE STREQUAL Release)
  add_definitions(-DDEBUG_LEVEL=1)
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

include_directories(.)
aux_source_directory(. SRC_LIST)
aux_source_directory(sdlobj SDLOBJ_LIST)
aux_source_directory(common COMMON_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST} ${SDLOBJ_LIST} ${COMMON_LIST})
