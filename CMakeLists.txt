cmake_minimum_required(VERSION 3.1.3)

project(gts CXX)

set(BUILD_EXAMPLES TRUE CACHE BOOL "Whether to build examples or not")
set(CMAKE_CXX_STANDARD 11)

set(HEADER_FOLDER   "include")
set(EXAMPLE_FOLDER  "examples")

set(HEADER_FILES
    "${HEADER_FOLDER}/gts/ptree.h"
    "${HEADER_FOLDER}/gts/gts.h"
    "${HEADER_FOLDER}/gts/common.h"
    "${HEADER_FOLDER}/gts/error.h"
    "${HEADER_FOLDER}/gts/bool.h"
    "${HEADER_FOLDER}/gts/integer.h"
    "${HEADER_FOLDER}/gts/float.h"
    "${HEADER_FOLDER}/gts/string.h"
    "${HEADER_FOLDER}/gts/json.h")

include_directories("${FASLIB_DIR}")
include_directories("${WJSON_DIR}")
include_directories("${DOUBLE_CONVERSION_DIR}")
link_directories("${DOUBLE_CONVERSION_DIR}/lib")

add_library(gts INTERFACE)
target_include_directories(gts INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${HEADER_FOLDER}>
    $<INSTALL_INTERFACE:include >
)

install(DIRECTORY "${HEADER_FOLDER}/" DESTINATION "include/")

if (${BUILD_EXAMPLES})
    add_executable(example 
        ${HEADER_FILES}
        ${EXAMPLE_FOLDER}/example.cpp)
    target_link_libraries(example gts double-conversion)
endif ()
