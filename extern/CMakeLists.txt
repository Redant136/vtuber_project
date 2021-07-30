add_subdirectory(glad/)
target_link_libraries(vtuber PRIVATE glad)

find_package(glfw QUIET)
if(glfw_found)
  target_link_libraries(vtuber PRIVATE glfw3)
else()
  add_subdirectory(glfw)
  target_link_libraries(vtuber PRIVATE glfw)
endif()

add_subdirectory(assimp)
target_link_libraries(vtuber PRIVATE assimp)

add_subdirectory(glm)
target_link_libraries(vtuber PRIVATE glm::glm)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)
target_link_libraries(vtuber PRIVATE nlohmann_json::nlohmann_json)


file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/../opencv_install")
set(opencv_install_command "
  cmake -DCMAKE_INSTALL_PREFIX=\"${CMAKE_BINARY_DIR}/../opencv_install\" ${CMAKE_BINARY_DIR}/../opencv_install;
  cmake --build . --config debug;
  cmake --build . --config debug --target install;
  cmake --build . --config release;
  cmake --build . --config release --target install")
execute_process(
  COMMAND "${opencv_install_command}"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/../opencv_install"
  RESULT_VARIABLE cmd_err)

if(WIN32)
  set_target_properties(vtuber PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
    ${CMAKE_BINARY_DIR}/exec)
  file(COPY "${CMAKE_BINARY_DIR}/../opencv_install/bin/Debug" DESTINATION "${CMAKE_BINARY_DIR}/exec")
  file(COPY "${CMAKE_BINARY_DIR}/../opencv_install/bin/Release" DESTINATION "${CMAKE_BINARY_DIR}/exec")
endif()

find_package(OpenCV REQUIRED PATHS "${CMAKE_BINARY_DIR}/..")

target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS})
# target_link_libraries(vtuber PRIVATE ${OpenCV_LIBS} "--static")
target_include_directories(vtuber PRIVATE ${OpenCV_INCLUDE_DIRS})

