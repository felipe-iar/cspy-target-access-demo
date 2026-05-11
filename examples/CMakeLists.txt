cmake_minimum_required (VERSION 3.25)

project(TargetAccessPluginClientExamples CXX)

if(NOT TOOLKIT_DIR)
  message(FATAL_ERROR "You must configure with -DTOOLKIT_DIR=\"/iar/ewarm-x.xx/arm\"")
endif()

find_library(TAC_Lib
  NAMES TargetAccessClientSDK
  PATHS "${TOOLKIT_DIR}/src/TargetAccessPlugin/lib"
        "${TOOLKIT_DIR}/../common/bin"
  REQUIRED)

add_executable(mem-reader mem-reader.cpp)
add_executable(mem-writer mem-writer.cpp)
add_executable(itm-logger itm-logger.cpp)

if(MSVC)
  set_target_properties(
    mem-reader
    mem-writer
    itm-logger
    PROPERTIES
      INCLUDE_DIRECTORIES "${TOOLKIT_DIR}/src/TargetAccessPlugin/inc"
      COMPILE_OPTIONS /Zi
      LINK_OPTIONS /SUBSYSTEM:console
      LINK_LIBRARIES ${TAC_Lib} )
endif()

if(UNIX)
  set_target_properties(
    mem-reader
    mem-writer
    itm-logger
    PROPERTIES
      INCLUDE_DIRECTORIES "${TOOLKIT_DIR}/src/TargetAccessPlugin/inc"
      LINK_LIBRARIES ${TAC_Lib} )
endif()

add_custom_command(TARGET itm-logger POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${TOOLKIT_DIR}/../common/bin/${CMAKE_SHARED_LIBRARY_PREFIX}TargetAccessClientSDK${CMAKE_SHARED_LIBRARY_SUFFIX}"
  $<TARGET_FILE_DIR:itm-logger>)
