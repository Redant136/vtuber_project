add_subdirectory(glad/)
target_link_libraries(vtuber PRIVATE glad)

# find_package(glfw)
# if(glfw_found)
#   target_link_libraries(vtuber PRIVATE glfw3)
# else()
  add_subdirectory(glfw)
  target_link_libraries(vtuber PRIVATE glfw)
# endif()

add_subdirectory(assimp)
target_link_libraries(vtuber PRIVATE assimp)

add_subdirectory(glm)
target_link_libraries(vtuber PRIVATE glm::glm)


# include(ExternalProject)
# ExternalProject_Add(OpenCV
#   SOURCE_DIR opencv
#   INSTALL_DIR opencv_build
#   INSTALL_COMMAND "cmake --build . --target install")
# # find_package(OpenCV REQUIRED)

# # target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
# # target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})

# set(BUILD_SHARED_LIBS OFF)
set(OpenCV_STATIC OFF)
# set(OPENCV_GENERATE_PKGCONFIG YES)
# find_package(OpenCV Required)

add_subdirectory(opencv)
# set(OpenCV_INCLUDE_DIRS "${CMAKE_BINARY_DIR};C:/Users/antoi/Documents/vtuber_project/extern/opencv/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/core/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/flann/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/imgproc/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/ml/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/photo/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/dnn/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/features2d/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/imgcodecs/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/videoio/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/calib3d/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/highgui/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/objdetect/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/stitching/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/ts/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/video/include;C:/Users/antoi/Documents/vtuber_project/extern/opencv/modules/gapi/include")
set(OpenCV_INCLUDE_DIRS "${CMAKE_BINARY_DIR};opencv/include;opencv/modules/core/include;opencv/modules/flann/include;opencv/modules/imgproc/include;opencv/modules/ml/include;opencv/modules/photo/include;opencv/modules/dnn/include;opencv/modules/features2d/include;opencv/modules/imgcodecs/include;opencv/modules/videoio/include;opencv/modules/calib3d/include;opencv/modules/highgui/include;opencv/modules/objdetect/include;opencv/modules/stitching/include;opencv/modules/ts/include;opencv/modules/video/include;opencv/modules/gapi/include")
set(OpenCV_LIBS "opencv_calib3d;opencv_core;opencv_dnn;opencv_features2d;opencv_flann;opencv_gapi;opencv_highgui;opencv_imgcodecs;opencv_imgproc;opencv_ml;opencv_objdetect;opencv_photo;opencv_stitching;opencv_video;opencv_videoio")

# find_package(OpenCV REQUIRED PATHS "${CMAKE_BINARY_DIR}/../install" "${CMAKE_BINARY_DIR}")

# MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
# MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

# target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS} "-static")
target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})











# macro(opencv_module_import)
# # target_link_directories(vtuber PRIVATE module/opencv_${ARGV0})
# target_link_libraries(vtuber PRIVATE opencv_${ARGV0}453d)
# # target_link_directories(vtuber PRIVATE extern/opencv/module/${ARGV0})

# file(COPY "${OpenCV_SOURCE_DIR}/modules/${ARGV0}/include/" DESTINATION "${CMAKE_BINARY_DIR}")
# endmacro()

# opencv_module_import(core)
# opencv_module_import(imgproc)
# opencv_module_import(photo)
# opencv_module_import(video)
# opencv_module_import(videoio)
# opencv_module_import(highgui)
# opencv_module_import(objdetect)
# opencv_module_import(gapi)
# opencv_module_import(imgcodecs)

# target_include_directories(vtuber PRIVATE "${CMAKE_BINARY_DIR}/")