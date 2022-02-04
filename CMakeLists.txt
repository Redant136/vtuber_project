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
  set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-unused-function -Wno-missing-braces")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-sign-compare -Wno-unused-function -Wno-missing-braces")
  set(CMAKE_C_FLAGS_DEBUG "-g")
  set(CMAKE_C_FLAGS_RELEASE "-O3")

  
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  message(STATUS "MSVC detected, adding compile flags")

  set(CMAKE_CXX_FLAGS "-Wall -Wno-sign-compare -Wno-unused-function")
  # set(CMAKE_CXX_FLAGS_DEBUG "-DEBUG")
  # set(CMAKE_CXX_FLAGS_RELEASE "-Ox")

endif()

if(APPLE)
  include_directories("/usr/local/include") 
  link_directories("/usr/local/lib")
endif(APPLE)

# -----------------------------

add_executable(vtuber main.cpp)
add_executable(faceDetection faceDetection.cpp)
add_executable(test webcamTest.cpp)

file(COPY models DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})

# ------------------------------


add_subdirectory(extern/glad/)

find_package(OpenGL REQUIRED)

find_package(glfw QUIET NAMES glfw glfw3)
if(NOT glfw_FOUND)
  add_subdirectory(extern/glfw)
endif()

# add_definitions(-DUSE_SSE4_INSTRUCTIONS=ON)
add_definitions(-DUSE_AVX_INSTRUCTIONS=ON)
add_subdirectory(extern/dlib)

# find_package(libwebcam REQUIRED NAMES libwebcam)

add_subdirectory(extern/glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json)

find_package(OpenCV REQUIRED HINTS PATHS "${CMAKE_SOURCE_DIR}/opencv_install")

# set(libtensorflowPath "${CMAKE_SOURCE_DIR}/libtensorflow/libtensorflow-cpu-linux-x86_64-2.7.0")
# add_library(tensorflow SHARED IMPORTED)
# set_target_properties(tensorflow PROPERTIES
#     IMPORTED_LOCATION ${libtensorflowPath}/lib/libtensorflow.so
#     INTERFACE_INCLUDE_DIRECTORIES ${libtensorflowPath}/include/
#   )

# ----------------------------------

add_library(libwebcam)

if(WIN32)
  target_sources(libwebcam PRIVATE webcam_opencv.cpp)
  target_link_libraries(libwebcam PRIVATE ${OpenCV_LIBS})
  target_include_directories(libwebcam PRIVATE ${OpenCV_INCLUDE_DIRS})
elseif(APPLE)
  target_sources(libwebcam PRIVATE webcam_opencv.cpp)
  target_link_libraries(libwebcam PRIVATE ${OpenCV_LIBS})
  target_include_directories(libwebcam PRIVATE ${OpenCV_INCLUDE_DIRS})
elseif(UNIX)
  # target_sources(libwebcam PRIVATE webcam_opencv.cpp)
  # target_link_libraries(libwebcam PRIVATE ${OpenCV_LIBS})
  # target_include_directories(libwebcam PRIVATE ${OpenCV_INCLUDE_DIRS})

  # find_path( AVUTIL_INCLUDE_DIR libavutil/avutil.h
  #   HINTS
  #   $ENV{AVUTILDIR}
  #   PATH_SUFFIXES include 
  #   PATHS
  #   ~/Library/Frameworks
  #   /Library/Frameworks
  #   /usr/local/include
  #   /usr/include
  #   /usr/include/ffmpeg
  #   /sw/include
  #   /opt/local/include
  #   /opt/csw/include 
  #   /opt/include
  #   /mingw/include
  # )

  # set(LIBAV_LIBS avcodec avformat avdevice swscale avfilter avutil)
  # target_sources(libwebcam PRIVATE webcam_av.c)
  # target_link_libraries(libwebcam PRIVATE ${LIBAV_LIBS})
  # target_include_directories(libwebcam PRIVATE ${AVUTIL_INCLUDE_DIR})

  target_sources(libwebcam PRIVATE webcam_v4l2.c)

endif()
# -----------------------------------------

target_link_libraries(vtuber PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(vtuber PRIVATE glad)
target_link_libraries(vtuber PRIVATE OpenGL::GL)
target_link_libraries(vtuber PRIVATE glfw)
target_link_libraries(vtuber PRIVATE glm::glm)
target_link_libraries(vtuber PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})

# target_link_libraries(vtuber PRIVATE ${LIBAV_LIBS})
# target_include_directories(vtuber PRIVATE ${LIBAV_PATH})

target_link_libraries(faceDetection PRIVATE ${CMAKE_DL_LIBS})
target_link_libraries(faceDetection PRIVATE ${OpenCV_LIBS})
target_include_directories(faceDetection PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(faceDetection PRIVATE dlib::dlib)
target_link_libraries(faceDetection PRIVATE libwebcam)



# target_link_libraries(test PUBLIC tensorflow)

target_link_libraries(test PRIVATE ${OpenCV_LIBS})
target_include_directories(test PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test PRIVATE libwebcam)
