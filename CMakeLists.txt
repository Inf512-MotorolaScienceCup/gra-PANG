cmake_minimum_required(VERSION 3.10)

project(Pang)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT RAYLIB_INCLUDE_DIR)
  set(RAYLIB_INCLUDE_DIR "raylib/include")
endif()

if(NOT RAYLIB_LIB_DIR)
  set(RAYLIB_LIB_DIR "raylib/lib")
endif()

if(RAYLIB_INCLUDE_DIR)
  include_directories(BEFORE SYSTEM ${RAYLIB_INCLUDE_DIR})
else()
  message(STATUS "RAYLIB_INCLUDE_DIR is not defined!")
endif()

if(RAYLIB_LIB_DIR)
  link_directories(${RAYLIB_LIB_DIR})
else()
  message(STATUS "RAYLIB_LIB_DIR is not defined!")
endif()

set(game_sources)
foreach(dir ./Pang)
  file(GLOB sources ${dir}/*.cpp)
  list(APPEND game_sources ${sources})
endforeach ()

set(game_resources)
file(GLOB resources "./Pang/res/*")
list(APPEND game_resources ${resources})

add_executable(pang ${game_sources})
target_link_libraries(pang raylib)
target_link_libraries(pang dl)
target_link_libraries(pang pthread)

file(COPY ${game_resources} DESTINATION "res/")
make_directory("saves")