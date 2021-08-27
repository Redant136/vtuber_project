
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

// #include <opencv2/core.hpp>
// #include <opencv2/gapi.hpp>
// #include <opencv2/photo.hpp>
// #include <opencv2/videoio.hpp>
// #include <opencv2/imgcodecs.hpp>
// #include <opencv2/highgui.hpp>

// #include "learnopengl/shader_m.h"
// #include "learnopengl/camera.h"
// #include "learnopengl/model.h"

// #include <iostream>

// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// // camera
// Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
// bool firstMouse = true;

// // timing
// float deltaTime = 0.0f;
// float lastFrame = 0.0f;

// int main()
// {
//   // glfw: initialize and configure
//   // ------------------------------
//   glfwInit();
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//   // glfw window creation
//   // --------------------
//   GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//   if (window == NULL)
//   {
//     std::cout << "Failed to create GLFW window" << std::endl;
//     glfwTerminate();
//     return -1;
//   }
//   glfwMakeContextCurrent(window);
//   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//   glfwSetCursorPosCallback(window, mouse_callback);
//   glfwSetScrollCallback(window, scroll_callback);

//   // tell GLFW to capture our mouse
//   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//   // glad: load all OpenGL function pointers
//   // ---------------------------------------
//   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//   {
//     std::cout << "Failed to initialize GLAD" << std::endl;
//     return -1;
//   }

//   // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//   stbi_set_flip_vertically_on_load(true);

//   // configure global opengl state
//   // -----------------------------
//   glEnable(GL_DEPTH_TEST);

//   // build and compile shaders
//   // -------------------------
//   Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

//   // load models
//   // -----------
//   Model ourModel("models/backpack.obj");

//   // draw in wireframe
//   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//   // render loop
//   // -----------
//   while (!glfwWindowShouldClose(window))
//   {
//     // per-frame time logic
//     // --------------------
//     float currentFrame = glfwGetTime();
//     deltaTime = currentFrame - lastFrame;
//     lastFrame = currentFrame;

//     // input
//     // -----
//     processInput(window);

//     // render
//     // ------
//     glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // don't forget to enable shader before setting uniforms
//     ourShader.use();

//     // view/projection transformations
//     glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//     glm::mat4 view = camera.GetViewMatrix();
//     ourShader.setMat4("projection", projection);
//     ourShader.setMat4("view", view);

//     // render the loaded model
//     glm::mat4 model = glm::mat4(1.0f);
//     model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//     model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
//     ourShader.setMat4("model", model);
//     ourModel.Draw(ourShader);

//     // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//     // -------------------------------------------------------------------------------
//     glfwSwapBuffers(window);
//     glfwPollEvents();
//   }

//   // glfw: terminate, clearing all previously allocated GLFW resources.
//   // ------------------------------------------------------------------
//   glfwTerminate();
//   return 0;
// }

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     glfwSetWindowShouldClose(window, true);

//   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//     camera.ProcessKeyboard(FORWARD, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//     camera.ProcessKeyboard(BACKWARD, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//     camera.ProcessKeyboard(LEFT, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//     camera.ProcessKeyboard(RIGHT, deltaTime);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//   // make sure the viewport matches the new window dimensions; note that width and
//   // height will be significantly larger than specified on retina displays.
//   glViewport(0, 0, width, height);
// }

// // glfw: whenever the mouse moves, this callback is called
// // -------------------------------------------------------
// void mouse_callback(GLFWwindow *window, double xpos, double ypos)
// {
//   if (firstMouse)
//   {
//     lastX = xpos;
//     lastY = ypos;
//     firstMouse = false;
//   }

//   float xoffset = xpos - lastX;
//   float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

//   lastX = xpos;
//   lastY = ypos;

//   camera.ProcessMouseMovement(xoffset, yoffset);
// }

// // glfw: whenever the mouse scroll wheel scrolls, this callback is called
// // ----------------------------------------------------------------------
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
// {
//   camera.ProcessMouseScroll(yoffset);
// }










// #define VMODEL "models/Avocado.glb"
#define VMODEL "models/DamagedHelmet.glb"
// #define VMODEL "models/AntiqueCamera.glb"

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

int main(){

  // vtuber::Importer importer = vtuber::Importer();
  // importer.import(VMODEL);

  VModel vmodel;
  Shader shader;
  launch(
      [&vmodel, &shader](GLFWwindow *window)
      {
        vmodel.loadModel(VMODEL);
        shader = Shader(VERTEX_SHADER, FRAGMENT_SHADER);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                                 {
                                   if (firstMouse)
                                   {
                                     lastX = xpos;
                                     lastY = ypos;
                                     firstMouse = false;
                                   }

                                   float xoffset = xpos - lastX;
                                   float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

                                   lastX = xpos;
                                   lastY = ypos;

                                   camera.ProcessMouseMovement(xoffset, yoffset);
                                 });
        glEnable(GL_DEPTH_TEST);

        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      },

      [&vmodel, &shader](GLFWwindow *window)
      {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.f, 1.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        //     // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            shader.setMat4("projection", projection);
            shader.setMat4("view", view);

        //     // render the loaded model
        //     glm::mat4 model = glm::mat4(1.0f);
        //     model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //     model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        //     ourShader.setMat4("model", model);
        //     ourModel.Draw(ourShader);

        // glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        // glm::vec3 camPos=glm::vec3(0,0,0);
        // glm::mat4 view = glm::lookAt(camPos, camPos + glm::vec3(1, 0, 0), glm::vec3(0, 1, 0));
        // shader.setMat4("projection", projection);
        // shader.setMat4("view", view);

        // shader.setMat4("projection", glm::mat4(1));
        // shader.setMat4("view", glm::mat4(1));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4("model", model);

        vmodel.draw(shader);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
          glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
          camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
          camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
          camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
          camera.ProcessKeyboard(RIGHT, deltaTime);

      });

  return 0;
}
