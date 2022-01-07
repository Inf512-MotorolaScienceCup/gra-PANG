cmake_minimum_required(VERSION 3.10)

project(Pang)

if(RAYLIB_INCLUDE_DIR)
  include_directories(BEFORE SYSTEM ${RAYLIB_INCLUDE_DIR})
else()
  message(STATUS "RAYLIB_INCLUDE_DIR is not defined!")
endif()

if(RAYLIB_LIB_DIR)
  link_directories(${RAYLIB_LIB_DIR})
  link_directories(${RAYLIB_LIB_DIR}/external/glfw/src)
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

# Windows
target_link_libraries(pang winmm.lib)

# Linux
#target_link_libraries(pang glfw3)
#target_link_libraries(pang dl)
#target_link_libraries(pang pthread)

file(COPY ${game_resources} DESTINATION "res/")
