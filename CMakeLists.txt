cmake_minimum_required (VERSION 3.15)
project(gnc LANGUAGES C)

include(CTest)

if(CMAKE_C_COMPILER_ID STREQUAL GNU)
  add_compile_options(-mtune=native -Warray-bounds)
endif()

add_executable(gnc)
target_link_libraries(gnc PRIVATE m)
add_subdirectory(src)

if(NOT BUILD_TESTING)
  return()
endif()

if(UNIX)
  add_test(NAME basic
    COMMAND ./DemoGNC.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_tests_properties(basic PROPERTIES TIMEOUT 15)
endif(UNIX)

find_package(Python COMPONENTS Interpreter)
if(Python_FOUND)
add_test(NAME Python
  COMMAND ${Python_EXECUTABLE} BlackRobustFlow.py data/pepsi
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()


find_package(Matlab COMPONENTS MAIN_PROGRAM)
if(Matlab_FOUND)
  add_test(NAME Matlab
          COMMAND ${Matlab_MAIN_PROGRAM} -batch "BlackRobustFlow('data/pepsi')"
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
