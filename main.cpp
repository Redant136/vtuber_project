
// #define VMODEL "models/Avocado.gltf"
// #define VMODEL "models/DamagedHelmet.glb"
// #define VMODEL "models/AntiqueCamera.glb"
// #define VMODEL "models/simple_morph_target.gltf"
// #define VMODEL "models/eva/scene.gltf"
// #define VMODEL "models/wave/scene.gltf"
// #define VMODEL "models/leopard/scene.gltf"

#include "learnopengl/camera.h"
#include "vtuber.hpp"
#include "vrmLoader.hpp"
using namespace vtuber;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
  launch2();
  // VModel vmodel;
  // launch(
  //     [&vmodel](GLFWwindow *window)
  //     {
  //       // vmodel.loadModel(VMODEL,vtuber::Filetype::gltf);
  //       vmodel.loadModel(VMODEL);

  //       vmodel.animate(0);

  //       glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  //       glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
  //                                {
  //                                  if (firstMouse)
  //                                  {
  //                                    lastX = xpos;
  //                                    lastY = ypos;
  //                                    firstMouse = false;
  //                                  }

  //                                  float xoffset = xpos - lastX;
  //                                  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  //                                  lastX = xpos;
  //                                  lastY = ypos;

  //                                  camera.ProcessMouseMovement(xoffset, yoffset); });

  //       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //     },

  //     [&vmodel](GLFWwindow *window)
  //     {
  //       Shader&shader=vmodel.shader;
        
  //       float currentFrame = glfwGetTime();
  //       deltaTime = currentFrame - lastFrame;
  //       lastFrame = currentFrame;

  //       glClearColor(0.3f, 0.3f, 0.3f, 1.f);
  //       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //       shader.use();

  //       glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
  //       glm::mat4 view = camera.GetViewMatrix();
  //       shader.setMat4("projection", projection);
  //       shader.setMat4("view", view);

  //       glm::mat4 model = glm::mat4(1.0f);
  //       model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
  //       model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * ((float)01));
  //       shader.setMat4("model", model);
  //       shader.setMat4("node", model);
  //       shader.setVec4("lightSource", glm::vec4(10, 0, 0, 0.2));
  //       // shader.setInt("lightSourcesLength", 1);

  //       vmodel.draw();

  //       if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  //         glfwSetWindowShouldClose(window, true);
  //       if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  //         camera.ProcessKeyboard(FORWARD, deltaTime);
  //       if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  //         camera.ProcessKeyboard(BACKWARD, deltaTime);
  //       if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  //         camera.ProcessKeyboard(LEFT, deltaTime);
  //       if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  //         camera.ProcessKeyboard(RIGHT, deltaTime);
  //     });

  return 0;
}
