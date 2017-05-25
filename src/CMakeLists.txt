cmake_minimum_required (VERSION 2.8.12)
project(gnc C)

add_compile_options(-mtune=native -fexceptions -Warray-bounds)

add_executable(gnc utils.c image-io.c filter.c gnc.c pyramid.c pyramid-sor.c sor.c image_stats.c warp.c derivatives.c disconts.c warp-back.c)

target_link_libraries(gnc m)
