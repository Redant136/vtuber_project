cmake_minimum_required(VERSION 3.13)
project(VTuber VERSION 1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "GCC detected, adding compile flags")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
  endif()
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")

else()

  message(STATUS "cl detected, adding compile flags")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
  endif()
  set(CMAKE_CXX_FLAGS "-W3")
  set(CMAKE_CXX_FLAGS_DEBUG "-DEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

endif(CMAKE_COMPILER_IS_GNUCXX)

if(APPLE)
  include_directories ( /usr/local/include) 
  link_directories( /usr/local/lib)
endif()


add_executable(vtuber main.cpp)

add_subdirectory(extern/glad/)
target_link_libraries(vtuber PRIVATE glad)

find_package(glfw)
if(glfw_found)
  target_link_libraries(vtuber PRIVATE glfw3)
else()
  add_subdirectory(extern/glfw)
  target_link_libraries(vtuber PRIVATE glfw)
endif()
