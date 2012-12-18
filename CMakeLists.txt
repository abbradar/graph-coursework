project(graph-coursework)
cmake_minimum_required(VERSION 2.8)

# Additional modules dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build")

add_definitions(-std=c++11 -Wall -fstack-protector -funsafe-loop-optimizations -fPIE -mtune=native -march=native -ffast-math)

# Build type
set(CMAKE_BUILD_TYPE Debug)

# Platform specific
include(CheckIncludeFiles)
check_include_files(unistd.h HAS_UNISTD_H)
if(HAS_UNISTD_H)
  add_definitions(-D__POSIX__)
endif()

# Debug levels
if(CMAKE_BUILD_TYPE STREQUAL Release)
  add_definitions(-DDEBUG_LEVEL=3 -O0)
else()
  add_definitions(-DDEBUG_LEVEL=1 -O3 -funswitch-loops -fomit-frame-pointer)
endif()

# Various packages
find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIR})
find_package(SDL_image REQUIRED)
include_directories(${SDLIMAGE_INCLUDE_DIR})
find_package(SDL_ttf REQUIRED)
include_directories(${SDLTTF_INCLUDE_DIR})
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIR})
find_package(YamlCpp REQUIRED)
include_directories(${YAMLCPP_INCLUDE_DIR})
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

link_libraries(${SDL_LIBRARY} ${SDLIMAGE_LIBRARY} ${SDLTTF_LIBRARY}
  ${Boost_LIBRARIES} ${YAMLCPP_LIBRARY} ${EIGEN3_LIBRARY})

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories(src)

aux_source_directory(src SRC_LIST)
aux_source_directory(src/sdlobj SDLOBJ_LIST)
aux_source_directory(src/xparse XPARSE_LIST)
aux_source_directory(src/common COMMON_LIST)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(FLEX_ARGS "-d")
  set(BISON_ARGS "-tk")
else()
  set(FLEX_ARGS " ")
  set(BISON_ARGS " ")
endif()

BISON_TARGET(xparser src/xparse/parser.yy "${CMAKE_CURRENT_BINARY_DIR}/parser.cc" COMPILE_FLAGS ${BISON_ARGS})
FLEX_TARGET(xlexer src/xparse/lexer.ll "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc" COMPILE_FLAGS ${FLEX_ARGS})
ADD_FLEX_BISON_DEPENDENCY(xlexer xparser)
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SDLOBJ_LIST} ${COMMON_LIST} ${XPARSE_LIST}
  "${CMAKE_CURRENT_BINARY_DIR}/parser.cc" "${CMAKE_CURRENT_BINARY_DIR}/lexer.cc")

file(GLOB RESOURCES res/*)
file(INSTALL ${RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(DOXYFILE_IN "build/Doxyfile.in")
include(UseDoxygen)

# Generate LaTeX documentation

#include(UseLATEX)

#ADD_LATEX_DOCUMENT("doc/statement.tex" DEFAULT_PDF MANGLE_TARGET_NAMES)
#ADD_LATEX_DOCUMENT("doc/summary.tex" DEFAULT_PDF MANGLE_TARGET_NAMES)
