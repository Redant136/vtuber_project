#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <iostream>
#include <stdlib.h>
#include <assert.h>

#define WIDTH 1920
#define HEIGHT 1080

namespace vtuber
{
  typedef unsigned short ushort;
  typedef unsigned int uint;
  typedef unsigned long ulong;
  typedef unsigned long long ullong;
  typedef long long llong;

  template <typename T, typename L = size_t>
  struct Array
  {
    L length = 0;
    T *arr = NULL;
    Array() = default;
    T &operator[](ullong i)
    {
      assert(arr);
      assert(i < length + 1);
      return arr[i];
    }
    Array<T> clone()
    {
      T *a = new T[length];
      memcpy(a, arr, sizeof(T) * length);
      return {length, a};
    }
    void free()
    {
      delete[] arr;
      arr = NULL;
    }
  };

  float randf()
  {
    return ((float)rand()) / RAND_MAX;
  }

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
  }

  static void compileShaders(){

  }

  void launch()
  {
    if (!glfwInit())
    {
      assert(0 && "could not create context");
      return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);
    if (window == NULL)
    {
      glfwTerminate();
      assert(0 && "cannot create window");
    }
    glfwMakeContextCurrent(window);
    // glfwSetWindowPos(window, 0, 0);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
      assert(0 && "could not initialize GLAD");
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // compile shaders
    // init

    while (!glfwWindowShouldClose(window))
    {
      glfwMakeContextCurrent(window);
      glClear(GL_COLOR_BUFFER_BIT);


      // draw here
      glfwSwapInterval(1); //v-sync
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

}