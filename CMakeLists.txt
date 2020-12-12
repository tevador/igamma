cmake_minimum_required(VERSION 2.8.8)

project(igamma)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message(STATUS "Setting default build type: ${CMAKE_BUILD_TYPE}")
endif()

include(CheckCCompilerFlag)

function(add_flag flag)
  string(REPLACE "-" "_" supported_c ${flag}_c)
  check_c_compiler_flag(${flag} ${supported_c})
  if(${${supported_c}})
    message(STATUS "Setting C flag ${flag}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}" PARENT_SCOPE)
  endif()
endfunction()

add_flag("-maes")

add_executable(igamma
src/lambert_w.c
src/igamma.c
src/main.c)

target_link_libraries(igamma m)
