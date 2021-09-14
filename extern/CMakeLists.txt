add_subdirectory(glad/)
target_link_libraries(vtuber PRIVATE glad)

find_package(OpenGL REQUIRED)
target_link_libraries(vtuber PRIVATE OpenGL::GL)


find_package(glfw QUIET NAMES glfw glfw3)
if(glfw_FOUND)
  target_link_libraries(vtuber PRIVATE glfw)
else()
  add_subdirectory(glfw)
  target_link_libraries(vtuber PRIVATE glfw)
endif()

add_subdirectory(glm)
target_link_libraries(vtuber PRIVATE glm::glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)
target_link_libraries(vtuber PRIVATE nlohmann_json::nlohmann_json)


find_package(OpenCV REQUIRED HINTS PATHS "${CMAKE_BINARY_DIR}/../extern" "${CMAKE_BINARY_DIR}/../opencv_install")
target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
# target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS} "--static")
target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})
