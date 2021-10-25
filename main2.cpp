#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include "vtuber.hpp"
#include <sstream>
using namespace cv;

class Shader
{
public:
  uint ID;
  std::string defines = "";
  Shader() : ID(0)
  {
  }
  Shader(const char *defines) : ID(0)
  {
    this->defines = defines;
  }
  Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr)
  {
    create(vertexPath, fragmentPath, geometryPath);
  }
  Shader(uint ID)
  {
    this->ID = ID;
  }
  Shader create(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr)
  {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode = "";
    std::string fragmentCode = "";
    std::string geometryCode = "";
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
      // open files
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // read file's buffer contents into streams
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // close file handlers
      vShaderFile.close();
      fShaderFile.close();
      // convert stream into string
      vertexCode = vShaderStream.str();
      vertexCode.insert(vertexCode.find("\n", vertexCode.find("#version")) + 1, defines);
      fragmentCode = fShaderStream.str();
      // if geometry shader path is present, also load a geometry shader
      if (geometryPath != nullptr)
      {
        gShaderFile.open(geometryPath);
        std::stringstream gShaderStream;
        gShaderStream << gShaderFile.rdbuf();
        gShaderFile.close();
        geometryCode = gShaderStream.str();
      }
    }
    catch (std::ifstream::failure &e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    uint vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryPath != nullptr)
    {
      const char *gShaderCode = geometryCode.c_str();
      geometry = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(geometry, 1, &gShaderCode, NULL);
      glCompileShader(geometry);
      checkCompileErrors(geometry, "GEOMETRY");
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryPath != nullptr)
      glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
      glDeleteShader(geometry);
    return *this;
  }
  void use()
  {
    glUseProgram(ID);
  }
  void setBool(const std::string &name, bool value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
  }
  void setInt(const std::string &name, int value) const
  {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setFloat(const std::string &name, float value) const
  {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
  }
  void setVec2(const std::string &name, const glm::vec2 &value) const
  {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec2(const std::string &name, float x, float y) const
  {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
  }
  void setVec3(const std::string &name, const glm::vec3 &value) const
  {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec3(const std::string &name, float x, float y, float z) const
  {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
  }
  void setVec4(const std::string &name, const glm::vec4 &value) const
  {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
  }
  void setVec4(const std::string &name, float x, float y, float z, float w)
  {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
  }
  void setMat2(const std::string &name, const glm::mat2 &mat) const
  {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  void setMat3(const std::string &name, const glm::mat3 &mat) const
  {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }
  void setMat4(const std::string &name, const glm::mat4 &mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
  }

  void setBoolArr(const std::string &name, uint size, bool *value) const
  {
    glUniform1iv(glGetUniformLocation(ID, name.c_str()), size, (int *)value);
  }
  void setIntArr(const std::string &name, uint size, int *value) const
  {
    glUniform1iv(glGetUniformLocation(ID, name.c_str()), size, value);
  }
  void setFloatArr(const std::string &name, uint size, float *value) const
  {
    glUniform1fv(glGetUniformLocation(ID, name.c_str()), size, value);
  }
  void setVec2Arr(const std::string &name, uint size, const float *value) const
  {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), size, value);
  }
  void setVec3Arr(const std::string &name, uint size, const float *value) const
  {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), size, value);
  }
  void setVec4Arr(const std::string &name, uint size, const float *value) const
  {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), size, value);
  }
  void setMat2Arr(const std::string &name, uint size, const float *mat) const
  {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), size, GL_FALSE, mat);
  }
  void setMat3Arr(const std::string &name, uint size, const float *mat) const
  {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), size, GL_FALSE, mat);
  }
  void setMat4Arr(const std::string &name, uint size, const float *mat) const
  {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), size, GL_FALSE, mat);
  }

private:
  void checkCompileErrors(GLuint shader, std::string type)
  {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
      glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
    else
    {
      glGetProgramiv(shader, GL_LINK_STATUS, &success);
      if (!success)
      {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
      }
    }
  }
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
unsigned char *cvMat2TexInput(Mat &img);

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for Mac OSX
#endif

  VideoCapture cap(0);
  if (!cap.isOpened())
  {
    std::cout << "Camera not opened!" << std::endl;
    return -1;
  }
  Mat frame;
  cap >> frame;
  int width = frame.cols;
  int height = frame.rows;
  unsigned char *image = cvMat2TexInput(frame);

  // glfw window creation
  GLFWwindow *window = glfwCreateWindow(width, height, "frame", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }


  const char shaderVert[]="#version 330 core\nlayout (location = 0) in vec3 aPos;layout (location = 1) in vec2 aTexCoord;out vec2 TexCoord;void main(){TexCoord=aTexCoord;gl_Position=vec4(aPos,1);}";
  const char shaderFrag[] = "#version 330 core\nin vec2 TexCoord;uniform sampler2D ;out vec4 FragColor;void main(){}";

  Shader ourShader("shader.vert", "shader.frag");
  float vertices[] = {
      //     Position       TexCoord
      -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f,   // top right
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // below left
      1.0f, -1.0f, 0.0f, 1.0f, 0.0f   // below right
  };
  // Set up index
  unsigned int indices[] = {
      0, 1, 2,
      1, 2, 3};

  unsigned int VAO, VBO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  while (!glfwWindowShouldClose(window))
  {
    cap >> frame;
    image = cvMat2TexInput(frame);
    if (image)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    else
    {
      std::cout << "Failed to load texture." << std::endl;
    }

    processInput(window);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Draw Rectangle
    ourShader.use();
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  (void)window;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

unsigned char *cvMat2TexInput(Mat &img)
{
  cvtColor(img, img, COLOR_BGR2RGB);
  flip(img, img, -1);
  return img.data;
}
