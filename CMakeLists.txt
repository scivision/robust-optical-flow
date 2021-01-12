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

add_test(NAME basic
  COMMAND $<TARGET_FILE:gnc> 11 12 4 1 data/yos results/flow- -l1 10.0 -l2 1.0 -S1 10.0 -S2 1.0 -s1 10.0 -s2 0.05 -stages 5 -nx 316 -ny 252 -f 0.5 -F 0 -w 1.995 -iters 20 -end ".pgm" -skip 15
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_tests_properties(basic PROPERTIES TIMEOUT 30)

find_package(Python COMPONENTS Interpreter)
if(Python_FOUND)
add_test(NAME Python
  COMMAND ${Python_EXECUTABLE} BlackRobustFlow.py data/pepsi
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_tests_properties(Python PROPERTIES TIMEOUT 60)
endif()


find_package(Matlab COMPONENTS MAIN_PROGRAM)
if(Matlab_FOUND)
  add_test(NAME Matlab
          COMMAND ${Matlab_MAIN_PROGRAM} -batch "BlackRobustFlow('data/pepsi')"
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
