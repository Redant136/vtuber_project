cmake_minimum_required(VERSION 3.13)
project(VTuber VERSION 1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)


if(CMAKE_COMPILER_IS_GNUCXX)
  message(STATUS "GCC detected, adding compile flags")
  # if(NOT CMAKE_BUILD_TYPE)
  #   set(CMAKE_BUILD_TYPE Debug)
  # endif()
  # set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  # set(CMAKE_CXX_FLAGS_DEBUG "-g")
  # set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  
  
  
  else()
  
  message(STATUS "cl detected, adding compile flags")
  # set(CMAKE_DEBUG_POSTFIX "d")
  # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

  # set(CMAKE_CXX_FLAGS "-Wall")
  # set(CMAKE_CXX_FLAGS_DEBUG "-DEBUG")
  # set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

endif(CMAKE_COMPILER_IS_GNUCXX)

if(APPLE)
  include_directories ( /usr/local/include) 
  link_directories( /usr/local/lib)
endif()

add_executable(vtuber main.cpp)

file(COPY models DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

TARGET_LINK_LIBRARIES(vtuber PRIVATE ${CMAKE_DL_LIBS})
add_subdirectory(extern)
