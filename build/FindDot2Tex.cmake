# - this module looks for dot2tex
#
# Once done this will define
#
#  DOT2TEX_FOUND - system has dot2tex
#  DOT2TEX_EXECUTABLE - the dot2tex executable

find_program(DOT2TEX_EXECUTABLE
  NAMES
  dot2tex
  PATHS
)

# handle the QUIETLY and REQUIRED arguments and set DOT2TEX_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(dot2tex
                                  REQUIRED_VARS DOT2TEX_EXECUTABLE)

mark_as_advanced(DOT2TEX_EXECUTABLE)

macro(DOT2TEX_TARGET Name FileInput FileOutput)
  set(DOT2TEX_TARGET_cmdopt "")

  if(${ARGC} GREATER 5 OR ${ARGC} EQUAL 5)
    if("${ARGV3}" STREQUAL "FLAGS")
      set(DOT2TEX_TARGET_extraopts "${ARGV4}")
      separate_arguments(DOT2TEX_TARGET_extraopts)
      list(APPEND DOT2TEX_TARGET_cmdopt ${DOT2TEX_TARGET_extraopts})
    endif()
  endif()

  add_custom_command(OUTPUT "${FileOutput}"
    COMMAND ${DOT2TEX_EXECUTABLE}
    ARGS ${DOT2TEX_TARGET_cmdopt} -o "${CMAKE_CURRENT_BINARY_DIR}/${FileOutput}" ${FileInput}
    DEPENDS ${FileInput}
    COMMENT "[dot2tex][${Name}] Converting .dot with dot2tex"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()
