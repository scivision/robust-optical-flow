cmake_minimum_required (VERSION 3.3)
project(gnc C)
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake/Modules/)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_compile_options(-g -O0)
else()
  add_compile_options(-O3)
endif()

if(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  add_compile_options(-mtune=native -Warray-bounds)
endif()

add_executable(gnc utils.c image-io.c filter.c gnc.c pyramid.c pyramid-sor.c sor.c image_stats.c warp.c derivatives.c disconts.c warp-back.c)
target_link_libraries(gnc m)
add_test(NAME basic COMMAND ./DemoGNC.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
set_tests_properties(basic PROPERTIES TIMEOUT 15)


find_package(Octave)
if (OCTAVE_MAJOR_VERSION GREATER_EQUAL 4)
  add_test(NAME Matlab 
          COMMAND octave-cli -q --eval "BlackRobustFlow('data/pepsi')"
          WORKING_DIRECTORY ..)
endif()
