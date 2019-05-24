cmake_minimum_required(VERSION 3.1.0)

project(gts CXX)

set(WITH_TESTS TRUE CACHE BOOL "Whether to build and run tests or not")
set(CMAKE_CXX_STANDARD 11)

#find_package(Faslib REQUIRED)

if (WITH_TESTS)
    find_package(Boost REQUIRED)
    find_package(GTest MODULE REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (WITH_SAMPLES)
    find_package(Boost REQUIRED)
    add_subdirectory(samples)
endif ()

set(CPACK_PACKAGE_NAME "gts")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")

install(DIRECTORY include DESTINATION include)
