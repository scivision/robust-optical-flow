cmake_minimum_required (VERSION 3.3)
project(gnc C)

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
