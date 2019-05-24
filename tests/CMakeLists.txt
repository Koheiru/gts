
file(GLOB_RECURSE HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(unit_tests ${HEADERS} ${SOURCES})
target_link_libraries(unit_tests PRIVATE GTest::Main GTest::GTest)

add_test(unit_tests unit_tests)
