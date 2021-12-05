cmake_minimum_required(VERSION 3.13)
project(VTuber VERSION 1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

if((${CMAKE_CXX_COMPILER_ID} STREQUAL GNU) OR (${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang) OR (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang))
  message(STATUS "${CMAKE_CXX_COMPILER_ID} compiler detected, adding compile flags")
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
  endif()
  set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-unused-function")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  message(STATUS "MSVC detected, adding compile flags")

  set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-unused-function")
  # set(CMAKE_CXX_FLAGS_DEBUG "-DEBUG")
  # set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

endif()

if(APPLE)
  include_directories ( /usr/local/include) 
  link_directories( /usr/local/lib)
endif(APPLE)

add_executable(vtuber main.cpp)
add_executable(faceDetection faceDetection.cpp)
add_executable(test test.cpp)

file(COPY models DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

set(LIBAV_LIBS avcodec avformat avdevice swscale avfilter avutil)
if(NOT LIBAV_PATH)
    set(LIBAV_PATH /usr/include/ffmpeg/)
endif()


target_link_libraries(vtuber PRIVATE ${CMAKE_DL_LIBS})


add_subdirectory(extern/glad/)
find_package(OpenGL REQUIRED)
find_package(glfw QUIET NAMES glfw glfw3)
if(NOT glfw_FOUND)
  add_subdirectory(extern/glfw)
endif()
add_subdirectory(extern/glm)
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json)
find_package(OpenCV REQUIRED HINTS PATHS "${CMAKE_BINARY_DIR}/../opencv_install" "${CMAKE_BINARY_DIR}/../extern")


target_link_libraries(vtuber PRIVATE glad)
target_link_libraries(vtuber PRIVATE OpenGL::GL)
target_link_libraries(vtuber PRIVATE glfw)
target_link_libraries(vtuber PRIVATE glm::glm)
target_link_libraries(vtuber PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})

target_link_libraries(vtuber PRIVATE ${LIBAV_LIBS})
target_include_directories(vtuber PRIVATE ${LIBAV_PATH})


target_link_libraries(faceDetection PRIVATE ${OpenCV_LIBS})
target_include_directories(faceDetection PRIVATE ${OpenCV_INCLUDE_DIRS})

add_library(knn kneuralnetwork/knn.c kneuralnetwork/knn_NEAT.c kneuralnetwork/knn_DeepLearning.c)
target_link_libraries(test knn)

# target_link_libraries(test PRIVATE ${CMAKE_DL_LIBS})

# add_subdirectory(libtensorflow)
# target_link_libraries(test PUBLIC tensorflow)

# target_link_libraries(test PRIVATE ${OpenCV_LIBS})
# target_include_directories(test PRIVATE ${OpenCV_INCLUDE_DIRS})
# target_link_libraries(test PRIVATE ${LIBAV_LIBS})
# target_include_directories(test PRIVATE ${LIBAV_PATH})
