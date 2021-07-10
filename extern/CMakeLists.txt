add_subdirectory(glad/)
target_link_libraries(vtuber PRIVATE glad)

find_package(glfw)
if(glfw_found)
  target_link_libraries(vtuber PRIVATE glfw3)
else()
  add_subdirectory(glfw)
  target_link_libraries(vtuber PRIVATE glfw)
endif()

add_subdirectory(assimp)
target_link_libraries(vtuber PRIVATE assimp)


file(COPY "opencv/include/" DESTINATION "${CMAKE_BINARY_DIR}")
add_subdirectory(opencv)

target_link_directories(vtuber PRIVATE module/opencv_core)
file(COPY "${OpenCV_SOURCE_DIR}/modules/core/include/" DESTINATION "${CMAKE_BINARY_DIR}")
target_link_directories(vtuber PRIVATE module/opencv_photo)
file(COPY "${OpenCV_SOURCE_DIR}/modules/photo/include/" DESTINATION "${CMAKE_BINARY_DIR}")
target_link_directories(vtuber PRIVATE module/opencv_videoio)
file(COPY "${OpenCV_SOURCE_DIR}/modules/videoio/include/" DESTINATION "${CMAKE_BINARY_DIR}")

target_include_directories(vtuber PRIVATE "${CMAKE_BINARY_DIR}/")