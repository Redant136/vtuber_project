#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CHEVAN_UTILS_MACRO_MAGIC
#define CHEVAN_UTILS_VEC2 glm::vec2
#define CHEVAN_UTILS_VEC3 glm::vec3
#define CHEVAN_UTILS_VEC4 glm::vec4
void print(glm::mat4 mat);
#include "chevan_utils.h"
using namespace chevan_utils;
using namespace chevanut_print;
void print(glm::mat4 mat)
{
  printMat4((float *)&mat);
}
#define print println

#include "vrmLoader.hpp"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#ifndef VMODEL
#define VMODEL "models/male1.glb"
// #define VMODEL "models/AliciaSolid_vrm-0.51.vrm"
// #define VMODEL "models/1565609261024596092.vrm"
#endif
#define GLTF_SHADER "shaders/vertex_shader.vert", "shaders/fragment_shader.frag"
#define MTOON_SHADER "shaders/MToon_shader.vert", "shaders/MToon_shader.frag", "shaders/MToon_shader.geom"

namespace vtuber
{
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

  enum Camera_Movement
  {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
  };
  class ModelCamera
  {
  public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    ModelCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -PIf / 2, float pitch = 0) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.f)
    {
      Position = position;
      WorldUp = up;
      Yaw = yaw;
      Pitch = pitch;
      updateCameraVectors();
    }
    ModelCamera(glm::vec3 position, glm::vec3 front, glm::vec3 up) : MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.f)
    {
      Position = position;
      WorldUp = up;
      Front = front;
      Yaw = std::atan(front.z / front.x);
      Pitch = std::asin(front.y);
      updateCameraVectors();
    }
    // constructor with scalar values
    ModelCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.f)
    {
      Position = glm::vec3(posX, posY, posZ);
      WorldUp = glm::vec3(upX, upY, upZ);
      Yaw = yaw;
      Pitch = pitch;
      updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
      return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
      float velocity = MovementSpeed * deltaTime;
      if (direction == FORWARD)
        Position += Front * velocity;
      if (direction == BACKWARD)
        Position -= Front * velocity;
      if (direction == LEFT)
        Position -= Right * velocity;
      if (direction == RIGHT)
        Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
      xoffset *= MouseSensitivity / 180 * PIf;
      yoffset *= MouseSensitivity / 180 * PIf;

      Yaw += xoffset;
      Pitch += yoffset;

      // make sure that when pitch is out of bounds, screen doesn't get flipped
      if (constrainPitch)
      {
        if (Pitch >= PIf / 2)
          Pitch = PIf / 2;
        if (Pitch <= -PIf / 2)
          Pitch = -PIf / 2;
      }

      // update Front, Right and Up Vectors using the updated Euler angles
      updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
      Zoom -= (float)yoffset;
      if (Zoom < 1.0f)
        Zoom = 1.0f;
      if (Zoom > 45.0f)
        Zoom = 45.0f;
    }

  private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
      // calculate the new Front vector
      glm::vec3 front;
      front.x = cos(Yaw) * cos(Pitch);
      front.y = sin(Pitch);
      front.z = sin(Yaw) * cos(Pitch);
      if (std::abs(Pitch - PIf / 2) < 0.0001)
      {
        front.x = cos(Yaw) * 0.00001;
        front.z = sin(Yaw) * 0.00001;
      }
      Front = glm::normalize(front);
      // also re-calculate the Right and Up vector
      Right = glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
      Up = glm::normalize(glm::cross(Right, Front));
    }
  };

  class VModel
  {
  public:
    gltf::glTFModel model;
    ModelCamera camera = ModelCamera(glm::vec3(0, 1, -1), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
    Array<uchar *> gltfBuffers;
    Array<uint> gltfBufferViewVBO;
    Array<uint> gltfMeshVAO;
    Array<uint> gltfImageTextureIndex;

    // pre-calc / optimization
    Array<bool> updatedNodeMorphs;
    Array<glm::mat4> nodeTransforms;
    Array<std::vector<uint>> nodeSkins;
    Array<glm::mat4> skiningMatrices;
    Array<bool> updatedNode;

    // rendering data
    uint current_alphaMode;
    enum class ShaderType
    {
      gltfShader,
      unlit,
      mtoon
    } shaderType = ShaderType::gltfShader;
    Shader shader;

    // animation
    struct
    {
      uint animation = -1;
      float animationStartTime = 0.f;
    } animationData;

    // vrm
    gltf::Extensions::VRM *vrmData = NULL;
    Array<gltf::Extensions::VRM::MaterialProperties *> vrmMaterialProperties = Array<gltf::Extensions::VRM::MaterialProperties *>(0);

    // camera stuff
    // cv::VideoCapture cameraCapture;
    // cv::Mat cameraFrame;
    // uint cameraFrameTexture;

    VModel()
    {
    }
    VModel(std::string path)
    {
      loadModel(path);
    }
    VModel(gltf::glTFModel model)
    {
      this->model = model;
    }
    ~VModel()
    {
      for (uint i = 0; i < gltfBuffers.length; i++)
        delete[] gltfBuffers[i];
      if (gltfMeshVAO.length)
        glDeleteVertexArrays(gltfMeshVAO.length, gltfMeshVAO.arr);
      if (gltfBufferViewVBO.length)
        glDeleteBuffers(gltfBufferViewVBO.length, gltfBufferViewVBO.arr);
      if (gltfImageTextureIndex.length)
        glDeleteTextures(gltfImageTextureIndex.length, gltfImageTextureIndex.arr);
      gltfMeshVAO.free();
      gltfBufferViewVBO.free();
      gltfImageTextureIndex.free();
      gltfBuffers.free();

      updatedNodeMorphs.free();
      nodeTransforms.free();
      nodeSkins.free();
      skiningMatrices.free();
      updatedNode.free();
    
      vrmMaterialProperties.free();
      gltf::freeModel(model);
    }

    void loadModel(std::string path, const Filetype type = Filetype::vrm)
    {
      Importer importer;
      importer.import(path, type);
      model = importer.model;

      if (gltf::findExtensionIndex("VRM", model) != -1)
      {
        vrmData = (gltf::Extensions::VRM *)model.extensions[gltf::findExtensionIndex("VRM", model)].data;
      }

      // Shader
      if (1)
      {
        std::string defines = "";
        uint MAX_JOINT_MATRIX = 1;
        for (uint i = 0; i < model.nodes.size(); i++)
        {
          if (model.nodes[i].skin != -1 && MAX_JOINT_MATRIX < model.skins[model.nodes[i].skin].joints.size())
          {
            MAX_JOINT_MATRIX = model.skins[model.nodes[i].skin].joints.size();
          }
        }
        defines += "#define MAX_JOINT_MATRIX " + std::to_string(MAX_JOINT_MATRIX) + "\n";

        if (gltf::findExtensionIndex("VRM", model) != -1)
        {
          std::string shaderName = vrmData->materialProperties[0].shader;
          if (shaderName.find("Unlit") != std::string::npos || shaderName.find("GLTFSHADER") != std::string::npos)
          {
            shaderType = ShaderType::unlit;
          }
          else if (shaderName.find("MToon") != std::string::npos)
          {
            shaderType = ShaderType::mtoon;
          }
          else
          {
            assert("Unknow shader" && 0);
          }
        }
        else
        {
          shaderType = ShaderType::gltfShader;
        }

        switch(shaderType){
          case ShaderType::gltfShader:
          case ShaderType::unlit:
            shader = Shader(defines.c_str()).create(GLTF_SHADER);
            break;
          case ShaderType::mtoon:
            shader = Shader(defines.c_str()).create(MTOON_SHADER);
            break;
          default:
            assert("Impossible" && 0);
        }
      }

      // buffers
      gltfBuffers = Array<uchar *>(model.buffers.size());
      updatedNode = Array<bool>(model.nodes.size());
      for (uint i = 0; i < model.buffers.size(); i++)
      {
        const gltf::Buffer &buffer = model.buffers[i];
        gltfBuffers[i] = new uchar[buffer.byteLength];
        memcpy(gltfBuffers[i], buffer.buffer, buffer.byteLength);
      }

      // setup morph weights and matrix and nodeMatrix
      updatedNodeMorphs = Array<bool>(model.nodes.size());
      for (uint i = 0; i < model.nodes.size(); i++)
      {
        updatedNodeMorphs[i] = true;
        updatedNode[i] = true;
        gltf::Node &node = model.nodes[i];
        if (node.weights.size() == 0)
        {
          uint maxSize = 0;
          if (node.mesh > -1)
          {
            for (gltf::Mesh::Primitive &primitive : model.meshes[node.mesh].primitives)
            {
              maxSize = (maxSize > primitive.targets.size()) ? maxSize : primitive.targets.size();
            }
          }
          node.weights = std::vector<float>(maxSize);
          for (uint j = 0; j < node.weights.size(); j++)
          {
            node.weights[j] = 0;
          }
          if (node.mesh > -1)
          {
            model.meshes[node.mesh].weights = std::vector<float>(maxSize);
          }
        }
        
      }

      // setup skinMatrix
      skiningMatrices = Array<glm::mat4>(model.nodes.size());
      nodeSkins = Array<std::vector<uint>>(model.nodes.size());
      for (uint i = 0; i < model.skins.size(); i++)
      {
        for(uint j=0;j<model.skins[i].joints.size();j++){
          nodeSkins[j].push_back(i);
        }
      }

      // VBOs
      gltfBufferViewVBO = Array<uint>(model.bufferViews.size());
      for (uint i = 0; i < model.bufferViews.size(); i++)
      {
        const gltf::BufferView &bufferView = model.bufferViews[i];

        int sparse_accessor = -1;
        for (uint a_i = 0; a_i < model.accessors.size(); a_i++)
        {
          const auto &accessor = model.accessors[a_i];
          if (accessor.bufferView == i)
          {
            if (accessor.sparse.count > 0)
            {
              sparse_accessor = a_i;
              break;
            }
          }
        }

        uint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(bufferView.target, VBO);
        if (sparse_accessor < 0)
          glBufferData(bufferView.target, bufferView.byteLength,
                       gltfBuffers[bufferView.buffer] + bufferView.byteOffset,
                       GL_STATIC_DRAW);
        else
        {
          const auto &accessor = model.accessors[sparse_accessor];
          // copy the buffer to a temporary one for sparse patching
          uchar *tmp_buffer = new uchar[bufferView.byteLength];
          memcpy(tmp_buffer, gltfBuffers[i] + bufferView.byteOffset,
                 bufferView.byteLength);

          const uint size_of_object_in_buffer =
              gltf::gltf_sizeof(accessor.componentType);
          const uint size_of_sparse_indices =
              gltf::gltf_sizeof(accessor.sparse.indices.componentType);

          const auto &indices_buffer_view =
              model.bufferViews[accessor.sparse.indices.bufferView];
          const auto &indices_buffer = model.buffers[indices_buffer_view.buffer];

          const auto &values_buffer_view =
              model.bufferViews[accessor.sparse.values.bufferView];
          const auto &values_buffer = model.buffers[values_buffer_view.buffer];

          for (uint sparse_index = 0; sparse_index < accessor.sparse.count;
               sparse_index++)
          {
            int index = 0;
            uchar *data = indices_buffer.buffer +
                          indices_buffer_view.byteOffset +
                          accessor.sparse.indices.byteOffset +
                          (sparse_index * size_of_sparse_indices);
            switch (accessor.sparse.indices.componentType)
            {
            case GLTF_COMPONENT_BYTE:
            case GLTF_COMPONENT_UBYTE:
              index = (int)*(uchar *)(data);
              break;
            case GLTF_COMPONENT_SHORT:
            case GLTF_COMPONENT_USHORT:
              index = (int)*(ushort *)(data);
              break;
            case GLTF_COMPONENT_INT:
            case GLTF_COMPONENT_UINT:
              index = (int)*(uint *)(data);
              break;
            case GLTF_COMPONENT_FLOAT:
            case GLTF_COMPONENT_DOUBLE:
            default:
              assert(0);
              break;
            }
            const uchar *read_from =
                values_buffer.buffer +
                (values_buffer_view.byteOffset +
                 accessor.sparse.values.byteOffset) +
                (sparse_index * (size_of_object_in_buffer * gltf::gltf_num_components(accessor.type)));

            uchar *write_to =
                tmp_buffer + index * (size_of_object_in_buffer * gltf::gltf_num_components(accessor.type));

            memcpy(write_to, read_from, size_of_object_in_buffer * gltf::gltf_num_components(accessor.type));
          }

          glBufferData(bufferView.target, bufferView.byteLength, tmp_buffer,
                       GL_STATIC_DRAW);
          delete[] tmp_buffer;
        }
        glBindBuffer(bufferView.target, 0);

        gltfBufferViewVBO[i] = VBO;
      }

      nodeTransforms = Array<glm::mat4>(model.nodes.size());
      for (uint i = 0; i < model.nodes.size(); i++)
      {
        nodeTransforms[i] = glm::mat4(1.f);
      }

      // Extensions
      if (gltf::findExtensionIndex("VRM", model) != -1)
      {
        typedef gltf::Extensions::VRM VRM;
        VRM vrm = *(VRM *)model.extensions[gltf::findExtensionIndex("VRM", model)].data;

        for (VRM::BlendShapeMaster::BlendShapeGroup blend : vrm.blendShapeMaster.blendShapeGroups)
        {
          if (blend.presetName == VRM::BlendShapeMaster::BlendShapeGroup::neutral)
          {
            for (VRM::BlendShapeMaster::BlendShapeGroup::Bind b : blend.binds)
            {
              model.meshes[b.mesh].weights[b.index] = b.weight / 100.f;
            }
          }
        }
      }

      update();

      // VAOs
      gltfMeshVAO = Array<uint>(model.meshes.size());
      for (uint i = 0; i < model.meshes.size(); i++)
      {
        const struct
        {
          const std::string accName;
          const int attribIndex;
        } attribs[9] = {{"POSITION", 0}, {"NORMAL", 1}, {"TANGENT", 2}, {"TEXCOORD_0", 3}, {"TEXCOORD_1", 4}, {"TEXCOORD_2", 5}, {"COLOR_0", 6}, {"JOINTS_0", 7}, {"WEIGHTS_0", 8}};

        const gltf::Mesh &mesh = model.meshes[i];
        uint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        for (uint j = 0; j < mesh.primitives.size(); j++)
        {
          const gltf::Mesh::Primitive &primitive = mesh.primitives[j];

          for (uint k = 0; k < sizeof(attribs) / sizeof(attribs[0]); k++)
          {
            if (gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName) == -1)
              continue;
            const gltf::Accessor &accessor = model.accessors[gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName)];
            glBindBuffer(GL_ARRAY_BUFFER, gltfBufferViewVBO[accessor.bufferView]);

            uint attribIndex = attribs[k].attribIndex;

            uint byteStride = 0;
            if (model.bufferViews[accessor.bufferView].byteStride == 0)
            {
              int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
              int numComponents = gltf::gltf_num_components(accessor.type);
              if (componentSizeInBytes <= 0)
                byteStride = -1;
              else if (numComponents <= 0)
                byteStride = -1;
              else
                byteStride = componentSizeInBytes * numComponents;
            }
            else
            {
              int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
              if (componentSizeInBytes <= 0)
                byteStride = -1;
              else if ((model.bufferViews[accessor.bufferView].byteStride % componentSizeInBytes) != 0)
                byteStride = -1;
              else
                byteStride = model.bufferViews[accessor.bufferView].byteStride;
            }
            model.bufferViews[accessor.bufferView].byteStride = byteStride;

            glVertexAttribPointer(attribIndex, gltf::gltf_num_components(accessor.type),
                                  accessor.componentType,
                                  accessor.normalized ? GL_TRUE : GL_FALSE,
                                  byteStride, reinterpret_cast<void *>(accessor.byteOffset));
            glEnableVertexAttribArray(attribIndex);
          }
        }
        gltfMeshVAO[i] = VAO;
      }

      // textures
      gltfImageTextureIndex = Array<uint>(model.images.size());
      for (uint i = 0; i < model.images.size(); i++)
      {
        const gltf::Image &image = model.images[i];
        int width, height, channels;
        uchar *im = NULL;
        if (image.bufferView != -1)
        {
          const gltf::BufferView &bufferView = model.bufferViews[image.bufferView];
          im = stbi_load_from_memory(model.buffers[bufferView.buffer].buffer + bufferView.byteOffset,
                                     bufferView.byteLength, &width, &height, &channels, 0);
          glDeleteBuffers(1, gltfBufferViewVBO + image.bufferView);
          gltfBufferViewVBO[image.bufferView] = 0;

          // NOTE(ANT) why doesnt this work??
          // glTexBuffer(GL_TEXTURE_2D, GL_RGB, gltfBufferViewVBO[image.bufferView]);
        }
        else if (image.uri.length() != 0)
        {
          std::string filename = importer.directory + "/" + image.uri;
          im = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        }
        else
        {
          width = 0;
          height = 0;
          channels = 0;
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (channels == 1)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RED,
                       GL_UNSIGNED_BYTE, im);
        }
        else if (channels == 3)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                       GL_UNSIGNED_BYTE, im);
        }
        else if (channels == 4)
        {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, im);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(im);

        gltfImageTextureIndex[i] = tex;
      }

      if (vrmData)
      {
        vrmMaterialProperties = Array<gltf::Extensions::VRM::MaterialProperties *>(model.materials.size());
        for (uint i = 0; i < vrmData->materialProperties.size(); i++)
        {
          for (uint j = 0; j < model.materials.size(); j++)
          {
            if (model.materials[j].name == vrmData->materialProperties[i].name)
            {
              vrmMaterialProperties[j] = &vrmData->materialProperties[i];
            }
          }
        }
      }
    }
    void initCam()
    {
      // int deviceID = 0;        // 0 = open default camera
      // cameraCapture.open(deviceID);
      // if (!cameraCapture.isOpened())
      // {
      //   std::cerr << "ERROR! Unable to open camera\n";
      //   return;
      // }
      // cameraCapture >> cameraFrame;
      // if (cameraFrame.empty())
      // {
      //   std::cerr << "ERROR! blank frame grabbed\n";
      // }

      // cv::flip(cameraFrame, cameraFrame, -1);
      // opengl stuff
      // print(cameraFrame.cols,",",cameraFrame.rows);
      // glGenTextures(1, &cameraFrameTexture);
      // glBindTexture(GL_TEXTURE_2D, cameraFrameTexture);

      // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cameraFrame.cols, cameraFrame.rows, 0, GL_BGR,
      //              GL_UNSIGNED_BYTE, cameraFrame.ptr());
      // glGenerateMipmap(GL_TEXTURE_2D);
    }
    void camMovement()
    {
      // cameraCapture >> cameraFrame;
      // if (cameraFrame.empty())
      // {
      //   std::cerr << "ERROR! blank frame grabbed\n";
      // }

      // cv::flip(cameraFrame, cameraFrame, -1);
      // glBindTexture(GL_TEXTURE_2D, cameraFrameTexture);
      // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, cameraFrame.cols, cameraFrame.rows, GL_BGR, GL_UNSIGNED_BYTE, cameraFrame.ptr());
      // glGenerateMipmap(GL_TEXTURE_2D);
    }
    void drawSkeleton()
    {
      std::vector<glm::vec3> nodes = std::vector<glm::vec3>();
      uint VBO, VAO;
      std::function<void(uint)> nodeSkeleton = [this, &nodeSkeleton, &nodes](uint node)
      {
        glm::mat4 mat = nodeTransforms[node];
        glm::vec3 p1 = mat * glm::vec4(0, 0, 0, 1);

        // if (model.nodes[node].skin > -1)
        // {
        //   for (uint i : model.skins[model.nodes[node].skin].joints)
        //   {
        //     glm::mat4 child = nodeTransforms[i];
        //     glm::vec3 p2 = child * glm::vec4(0, 0, 0, 1);

        //     nodes.push_back(p1);
        //     nodes.push_back(p2);
        //     nodeSkeleton(i);
        //   }
        // }
        for (uint i : model.nodes[node].children)
        {
          glm::mat4 child = nodeTransforms[i];
          glm::vec3 p2 = child * glm::vec4(0, 0, 0, 1);

          nodes.push_back(p1);
          nodes.push_back(p2);
          nodeSkeleton(i);
        }
      };

      const gltf::Scene &scene = model.scenes[model.scene > -1 ? model.scene : 0];
      for (size_t i = 0; i < scene.nodes.size(); i++)
      {
        nodeSkeleton(scene.nodes[i]);
      }

      shader.setVec4("baseColorFactor", glm::vec4(0, 0, 0, 1));
      glGenBuffers(1, &VBO);
      glGenVertexArrays(1, &VAO);
      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * nodes.size(), nodes.data(), GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
      glEnableVertexAttribArray(0);

      shader.setMat4("node", glm::mat4(1.f));
      Array<glm::mat4> jointMatrices = Array<glm::mat4>(128);
      for (uint i = 0; i < jointMatrices.length; i++)
      {
        jointMatrices[i] = glm::mat4(1.f);
      }
      glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_jointMatrix"), jointMatrices.length, GL_FALSE, (float *)jointMatrices.arr);
      shader.use();
      glDrawArrays(GL_LINES, 0, nodes.size() / 2);
      glDeleteVertexArrays(1, &VAO);
      glDeleteBuffers(1, &VBO);
      jointMatrices.free();
    }
    void draw()
    {
      shader.use();

      update();

      const gltf::Scene &scene = model.scenes[model.scene > -1 ? model.scene : 0];
      for (uint mode = gltf::Material::OPAQUE; mode <= gltf::Material::BLEND; mode++)
      {
        current_alphaMode = mode;
        for (uint i : scene.nodes)
        {
          drawNode(model.nodes[i], nodeTransforms[i]);
        }
      }
    }

    void update()
    {
      // animation
      if (animationData.animation != -1)
      {
        const gltf::Animation &animation = model.animations[animationData.animation];

        for (uint i = 0; i < animation.channels.size(); i++)
        {
          const gltf::Animation::AnimationChannel &channel = animation.channels[i];
          const gltf::Animation::AnimationSampler &sampler = animation.samplers[channel.sampler];

          const gltf::Accessor &input_accessor = model.accessors[sampler.input];
          const gltf::Accessor &output_accessor = model.accessors[sampler.output];

          gltf::Node &node = model.nodes[channel.target.node];

          std::vector<float> times;
          for (uint j = 0; j < input_accessor.count; j++)
          {
            times.push_back(*(float *)gltf::getDataFromAccessor(model, input_accessor, j));
          }

          float relativeTime = glfwGetTime() - animationData.animationStartTime;
          int animationFrame = -1;
          for (uint j = 0; j < times.size(); j++)
          {
            if (relativeTime > times[j])
            {
              animationFrame = j;
            }
          }

          if (channel.target.path == gltf::Animation::AnimationChannel::AnimationTarget::weights)
          {
            updatedNodeMorphs[i] = true;
            std::vector<float> weights;

            for (uint j = 0; j < output_accessor.count; j++)
              weights.push_back(*(float *)gltf::getDataFromAccessor(model, output_accessor, j));

            if (sampler.interpolation == gltf::Animation::AnimationSampler::STEP)
            {
              if (animationFrame != -1)
              {
                for (uint j = 0; j < node.weights.size(); j++)
                {
                  node.weights[j] = weights[(node.weights.size() * animationFrame) + j];
                }
              }
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::LINEAR)
            {
              if (animationFrame + 1 < times.size())
              {
                std::vector<float> previousWeight = std::vector<float>(node.weights.size());
                if (animationFrame < 0)
                  previousWeight = node.weights;
                else
                  memcpy(previousWeight.data(), weights.data() + (node.weights.size() * animationFrame), sizeof(float) * node.weights.size());
                std::vector<float> nextWeight = std::vector<float>(node.weights.size());
                memcpy(nextWeight.data(), weights.data() + (node.weights.size() * (animationFrame + 1)), sizeof(float) * node.weights.size());
                float interpolationValue = (relativeTime - times[animationFrame]) / (times[animationFrame + 1] - times[animationFrame]);
                std::vector<float> weight = std::vector<float>(node.weights.size());
                for (uint w = 0; w < weight.size(); w++)
                {
                  weight[w] = previousWeight[w] + interpolationValue * (nextWeight[w] - previousWeight[w]);
                }
                node.weights = weight;
              }
              else
              {
                if (animationFrame != -1)
                {
                  for (uint j = 0; j < node.weights.size(); j++)
                  {
                    node.weights[j] = weights[(node.weights.size() * animationFrame) + j];
                  }
                }
              }
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::CUBICSPLINE)
            {
              assert(0 && "not implemented");
            }
          }
          else if (channel.target.path == gltf::Animation::AnimationChannel::AnimationTarget::translation)
          {
            std::vector<glm::vec3> translations;
            for (uint j = 0; j < output_accessor.count; j++)
            {
              translations.push_back(*(glm::vec3 *)gltf::getDataFromAccessor(model, output_accessor, j));
            }
            if (node.translation.size() == 0)
              node.translation = std::vector<float>(3);

            if (sampler.interpolation == gltf::Animation::AnimationSampler::STEP)
            {
              if (animationFrame != -1)
                memcpy(node.translation.data(), &translations[animationFrame], sizeof(float) * 3);
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::LINEAR)
            {
              if (animationFrame + 1 < times.size())
              {
                glm::vec3 previousTranslation;
                if (animationFrame < 0)
                  memcpy(&previousTranslation, node.translation.data(), sizeof(float) * 3);
                else
                  previousTranslation = translations[animationFrame];
                glm::vec3 nextTranslation = translations[animationFrame + 1];
                float interpolationValue = (relativeTime - times[animationFrame]) / (times[animationFrame + 1] - times[animationFrame]);
                glm::vec3 translation = previousTranslation + interpolationValue * (nextTranslation - previousTranslation);
                memcpy(node.translation.data(), &translation, sizeof(float) * 3);
              }
              else
              {
                if (animationFrame != -1)
                  memcpy(node.translation.data(), &translations[animationFrame], sizeof(float) * 3);
              }
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::CUBICSPLINE)
            {
              assert(0 && "not implemented");
            }
          }
          else if (channel.target.path == gltf::Animation::AnimationChannel::AnimationTarget::rotation)
          {
            std::vector<glm::vec4> rotations;

            for (uint j = 0; j < output_accessor.count; j++)
            {
              rotations.push_back(*(glm::vec4 *)gltf::getDataFromAccessor(model, output_accessor, j));
            }

            if (node.rotation.size() == 0)
              node.rotation = std::vector<float>(4);

            if (sampler.interpolation == gltf::Animation::AnimationSampler::STEP)
            {
              if (animationFrame != -1)
                memcpy(node.rotation.data(), &rotations[animationFrame], sizeof(float) * 4);
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::LINEAR)
            {
              if (animationFrame + 1 < times.size())
              {
                float interpolationValue = (relativeTime - times[animationFrame]) / (times[animationFrame + 1] - times[animationFrame]);
                glm::vec4 previousRotation;
                if (animationFrame < 0)
                  memcpy(&previousRotation, node.rotation.data(), sizeof(float) * 4);
                else
                  previousRotation = rotations[animationFrame];
                glm::vec4 nextRotation = rotations[animationFrame + 1];
                float dot = glm::dot(previousRotation, nextRotation);
                glm::vec4 rotation;

                if (dot < 0)
                {
                  dot = -dot;
                  nextRotation = -nextRotation;
                }
                if (dot > 0.9995)
                  rotation = glm::normalize(previousRotation + interpolationValue * (nextRotation - previousRotation));
                else
                {
                  float theta_0 = glm::acos(dot);
                  float theta = interpolationValue * theta_0;
                  float sin_theta = glm::sin(theta);
                  float sin_theta_0 = glm::sin(theta_0);

                  float scalePreviousQuat = glm::cos(theta) - dot * sin_theta / sin_theta_0;
                  float scaleNextQuat = sin_theta / sin_theta_0;

                  rotation = scalePreviousQuat * previousRotation + scaleNextQuat * nextRotation;
                }

                memcpy(node.rotation.data(), &rotation, sizeof(float) * 4);
              }
              else
              {
                if (animationFrame != -1)
                  memcpy(node.rotation.data(), &rotations[animationFrame], sizeof(float) * 4);
              }
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::CUBICSPLINE)
            {
              assert(0 && "not implemented");
            }
          }
          else if (channel.target.path == gltf::Animation::AnimationChannel::AnimationTarget::scale)
          {
            std::vector<glm::vec3> scales;
            for (uint j = 0; j < output_accessor.count; j++)
            {
              scales.push_back(*(glm::vec3 *)gltf::getDataFromAccessor(model, output_accessor, j));
            }
            if (node.scale.size() == 0)
              node.scale = std::vector<float>(3);

            if (sampler.interpolation == gltf::Animation::AnimationSampler::STEP)
            {
              if (animationFrame != -1)
                memcpy(node.scale.data(), &scales[animationFrame], sizeof(float) * 3);
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::LINEAR)
            {
              if (animationFrame + 1 < times.size())
              {
                glm::vec3 previousScale;
                if (animationFrame < 0)
                  memcpy(&previousScale, node.scale.data(), sizeof(float) * 3);
                else
                  previousScale = scales[animationFrame];
                glm::vec3 nextScale = scales[animationFrame + 1];
                float interpolationValue = (relativeTime - times[animationFrame]) / (times[animationFrame + 1] - times[animationFrame]);
                glm::vec3 scale = previousScale + interpolationValue * (nextScale - previousScale);
                memcpy(node.scale.data(), &scale, sizeof(float) * 3);
              }
              else
              {
                if (animationFrame != -1)
                  memcpy(node.scale.data(), &scales[animationFrame], sizeof(float) * 3);
              }
            }
            else if (sampler.interpolation == gltf::Animation::AnimationSampler::CUBICSPLINE)
            {
              assert(0 && "not implemented");
            }
          }

          if (animationFrame >= times.size() - 1)
          {
            animationData.animation = -1;
            animationData.animationStartTime = 0;
          }
        }
      }

      // morphs
      for (uint i = 0; i < model.nodes.size(); i++)
      {
        if (updatedNodeMorphs[i])
        {
          updateMorph(model.nodes[i]);
          updatedNodeMorphs[i] = false;
        }
      }

      // matrices
      const gltf::Scene &scene = model.scenes[model.scene > -1 ? model.scene : 0];
      for (uint i : scene.nodes)
      {
        updateNodeMatrix(i);
      }
      for (uint i = 0; i < model.nodes.size(); i++)
      {
        if (updatedNode[i])
        {
          for (uint j = 0; j < nodeSkins[i].size(); j++)
          {
            updateSkinMatrix(nodeSkins[i][j]);
          }
        }
      }

      for (uint i = 0; i < model.nodes.size(); i++)
      {
        updatedNode[i] = false;
      }
      // NOTE(ANT) inverse kinematics
    }
    void animate(uint index)
    {
      if (index < model.animations.size())
      {
        animationData.animation = index;
        animationData.animationStartTime = glfwGetTime();
      }
    }

    void setExpression(std::string name)
    {
      if (vrmData)
      {
        using Expression = gltf::Extensions::VRM::BlendShapeMaster::BlendShapeGroup::PresetNames;
        Expression expression;
        name = toLowerCase(name);
        if (name == "a")
          expression = Expression::a;
        else if (name == "i")
          expression = Expression::i;
        else if (name == "u")
          expression = Expression::u;
        else if (name == "e")
          expression = Expression::e;
        else if (name == "o")
          expression = Expression::o;
        else if (name == "blink")
          expression = Expression::blink;
        else if (name == "joy" || name == "happy")
          expression = Expression::joy;
        else if (name == "angry")
          expression = Expression::angry;
        else if (name == "sorrow" || name == "sad")
          expression = Expression::sorrow;
        else if (name == "fun")
          expression = Expression::fun;
        else if (name == "lookup" || name == "look_up")
          expression = Expression::lookup;
        else if (name == "lookdown" || name == "look_down")
          expression = Expression::lookdown;
        else if (name == "lookleft" || name == "look_left")
          expression = Expression::lookleft;
        else if (name == "lookright" || name == "look_right")
          expression = Expression::lookright;
        else if (name == "blink_l")
          expression = Expression::blink_l;
        else if (name == "blink_r")
          expression = Expression::blink_r;
        else
          expression = Expression::unknown;
        for (uint i = 0; i < vrmData->blendShapeMaster.blendShapeGroups.size(); i++)
        {
          if (vrmData->blendShapeMaster.blendShapeGroups[i].presetName == expression)
          {
            vtuber::gltf::Extensions::VRM::BlendShapeMaster::BlendShapeGroup blend = vrmData->blendShapeMaster.blendShapeGroups[i];
            for (uint j = 0; j < blend.binds.size(); j++)
            {
              model.meshes[blend.binds[j].mesh].weights[blend.binds[j].index] = blend.binds[j].weight / 100.f;
              for (uint k = 0; k < model.nodes.size(); k++)
              {
                if (model.nodes[k].mesh == blend.binds[j].mesh)
                  updatedNodeMorphs[k] = true;
              }
            }
          }
        }
      }
    }

  private:
    static glm::mat4 getNodeTRS(const gltf::Node &node)
    {
      glm::mat4 trs = glm::mat4(1.f);

      if (node.scale.size() != 0)
      {
        trs = glm::scale(trs, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
      }
      if (node.rotation.size() != 0)
      {
        trs *= glm::mat4_cast(glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]));
      }
      if (node.translation.size() != 0)
      {
        trs = glm::translate(trs, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
      }
      return trs;
    }
    void updateMorph(gltf::Node &node)
    {
      if (node.mesh > -1)
      {
        std::vector<float> weights;
        if (node.weights.size() > 0)
          weights = node.weights;

        const gltf::Mesh &mesh = model.meshes[node.mesh];
        if (mesh.weights.size() > 0)
        {
          for (uint i = 0; i < weights.size(); i++)
          {
            weights[i] += mesh.weights[i];
          }
        }

        for (uint i = 0; i < mesh.primitives.size(); i++)
        {
          const gltf::Mesh::Primitive &primitive = mesh.primitives[i];
          if (weights.size() == 0)
            weights = std::vector<float>(primitive.targets.size());

          std::vector<glm::vec3> positionMorphs;
          if (primitive.attributes.POSITION != -1)
          {
            positionMorphs = std::vector<glm::vec3>(model.accessors[primitive.attributes.POSITION].count);
          }
          std::vector<glm::vec3> normalMorphs;
          if (primitive.attributes.NORMAL != -1)
          {
            normalMorphs = std::vector<glm::vec3>(model.accessors[primitive.attributes.NORMAL].count);
          }
          std::vector<glm::vec4> tangentMorphs;
          if (primitive.attributes.TANGENT != -1)
          {
            tangentMorphs = std::vector<glm::vec4>(model.accessors[primitive.attributes.TANGENT].count);
          }

          for (uint j = 0; j < primitive.targets.size(); j++)
          {
            float weight = weights[j];
            const gltf::Mesh::Primitive::MorphTarget &target = primitive.targets[j];

            if (target.POSITION != -1 && primitive.attributes.POSITION != -1)
            {
              const gltf::Accessor &target_acc = model.accessors[target.POSITION];
              for (uint k = 0; k < target_acc.count; k++)
              {
                glm::vec3 target_morph;
                memcpy(&target_morph, gltf::getDataFromAccessor(model, target_acc, k), sizeof(float) * 3);

                positionMorphs[k] += target_morph * weight;
              }
            }
            if (target.NORMAL != -1 && primitive.attributes.NORMAL != -1)
            {
              const gltf::Accessor &target_acc = model.accessors[target.NORMAL];
              for (uint k = 0; k < target_acc.count; k++)
              {
                glm::vec3 target_morph;
                memcpy(&target_morph, gltf::getDataFromAccessor(model, target_acc, k), sizeof(float) * 3);

                normalMorphs[k] += target_morph * weight;
              }
            }
            if (target.TANGENT != -1 && primitive.attributes.TANGENT != -1)
            {
              const gltf::Accessor &target_acc = model.accessors[target.NORMAL];
              for (uint k = 0; k < target_acc.count; k++)
              {
                glm::vec4 target_morph;
                memcpy(&target_morph, gltf::getDataFromAccessor(model, target_acc, k), sizeof(float) * 4);

                tangentMorphs[k] += target_morph * weight;
              }
            }
          }

          if (primitive.attributes.POSITION != -1)
          {
            const gltf::Accessor &accessor = model.accessors[primitive.attributes.POSITION];
            const gltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
            uint byteStride = bufferView.byteStride;
            if (byteStride == 0)
            {
              int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
              int numComponents = gltf::gltf_num_components(accessor.type);
              if (componentSizeInBytes <= 0)
                byteStride = -1;
              else if (numComponents <= 0)
                byteStride = -1;
              else
                byteStride = componentSizeInBytes * numComponents;
            }

            for (uint j = 0; j < accessor.count; j++)
            {
              glm::vec3 position;
              memcpy(&position, gltf::getDataFromAccessor(model, accessor, j), sizeof(float) * 3);

              position = position + positionMorphs[j];

              uchar *write_mem = (gltfBuffers[bufferView.buffer] + bufferView.byteOffset) + accessor.byteOffset + (byteStride)*j;

              if (memcmp(write_mem, &position, sizeof(float) * 3) != 0)
              {
                memcpy(write_mem, &position, sizeof(float) * 3);
                glBindBuffer(GL_ARRAY_BUFFER, gltfBufferViewVBO[accessor.bufferView]);
                glBufferSubData(GL_ARRAY_BUFFER, accessor.byteOffset + (byteStride)*j, sizeof(float) * 3, &position);
              }
            }
          }
          if (primitive.attributes.NORMAL != -1)
          {
            const gltf::Accessor &accessor = model.accessors[primitive.attributes.NORMAL];
            const gltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
            uint byteStride = bufferView.byteStride;
            if (byteStride == 0)
            {
              int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
              int numComponents = gltf::gltf_num_components(accessor.type);
              if (componentSizeInBytes <= 0)
                byteStride = -1;
              else if (numComponents <= 0)
                byteStride = -1;
              else
                byteStride = componentSizeInBytes * numComponents;
            }

            for (uint j = 0; j < accessor.count; j++)
            {
              glm::vec3 normal;
              memcpy(&normal, gltf::getDataFromAccessor(model, accessor, j), sizeof(float) * 3);
              normal += normalMorphs[j];

              uchar *write_mem = (gltfBuffers[bufferView.buffer] + bufferView.byteOffset) + accessor.byteOffset + (byteStride)*j;

              if (memcmp(write_mem, &normal, sizeof(float) * 3) != 0)
              {
                memcpy(write_mem, &normal, sizeof(float) * 3);
                glNamedBufferSubData(gltfBufferViewVBO[accessor.bufferView], accessor.byteOffset + (byteStride)*j, sizeof(float) * 3, &normal);
              }
            }
          }
          if (primitive.attributes.TANGENT != -1)
          {
            const gltf::Accessor &accessor = model.accessors[primitive.attributes.TANGENT];
            const gltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
            uint byteStride = bufferView.byteStride;
            if (byteStride == 0)
            {
              int componentSizeInBytes = gltf::gltf_sizeof(accessor.componentType);
              int numComponents = gltf::gltf_num_components(accessor.type);
              if (componentSizeInBytes <= 0)
                byteStride = -1;
              else if (numComponents <= 0)
                byteStride = -1;
              else
                byteStride = componentSizeInBytes * numComponents;
            }

            for (uint j = 0; j < accessor.count; j++)
            {
              glm::vec4 tangent;
              memcpy(&tangent, gltf::getDataFromAccessor(model, accessor, j), sizeof(float) * 4);
              tangent += tangentMorphs[j];

              uchar *write_mem = (gltfBuffers[bufferView.buffer] + bufferView.byteOffset) + accessor.byteOffset + (byteStride)*j;

              if (memcmp(write_mem, &tangent, sizeof(float) * 4) != 0)
              {
                memcpy(write_mem, &tangent, sizeof(float) * 4);
                glNamedBufferSubData(gltfBufferViewVBO[accessor.bufferView], accessor.byteOffset + (byteStride)*j, sizeof(float) * 4, &tangent);
              }
            }
          }
        }
      }
    }
    void updateNodeMatrix(uint node, glm::mat4 parentTransform = glm::mat4(1.f))
    {
      if (updatedNode[node])
      {
        glm::mat4 mat = parentTransform;
        if (model.nodes[node].matrix.size() != 0)
          mat *= glm::make_mat4((float *)model.nodes[node].matrix.data());
        mat *= getNodeTRS(model.nodes[node]);
        nodeTransforms[node] = mat;
      }

      for (uint i : model.nodes[node].children)
      {
        updateNodeMatrix(i, nodeTransforms[node]);
      }
    }
    void updateSkinMatrix(uint skin)
    {
      for (uint i = 0; i < model.skins[skin].joints.size(); i++)
      {
        glm::mat4 jointNodeMat = nodeTransforms[model.skins[skin].joints[i]];
        glm::mat4 inverseBindMatrix = glm::make_mat4((float *)gltf::getDataFromAccessor(model, model.accessors[model.skins[skin].inverseBindMatrices], i));
        skiningMatrices[model.skins[skin].joints[i]] = jointNodeMat * inverseBindMatrix;
      }
    }

    void bindTexture(const gltf::Texture &texture, uint &texCoord, uint sampler_obj, uint GL_TextureIndex)
    {
      if (gltf::findExtensionIndex("KHR_texture_transform", texture) != -1)
      {
        gltf::Extensions::KHR_texture_transform *ext = (gltf::Extensions::KHR_texture_transform *)texture.extensions[gltf::findExtensionIndex("KHR_texture_transform", texture)].data;
        texCoord = ext->texCoord;
        membuild(glm::vec2, offset, ext->offset.data());
        membuild(glm::vec2, scale, ext->scale.data());
        shader.setBool("KHR_texture_transform", true);
        shader.setVec2("u_offset", offset);
        shader.setFloat("u_rotation", ext->rotation);
        shader.setVec2("u_scale", scale);
      }
      else
      {
        shader.setBool("KHR_texture_transform", false);
      }

      const gltf::Sampler &sampler = model.samplers[texture.sampler];
      glSamplerParameterf(sampler_obj, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
      glSamplerParameterf(sampler_obj, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
      glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_S, sampler.wrapS);
      glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_T, sampler.wrapT);

      glActiveTexture(GL_TEXTURE0 + GL_TextureIndex);

      glBindTexture(GL_TEXTURE_2D, gltfImageTextureIndex[texture.source]);
      glBindSampler(GL_TEXTURE_2D, sampler_obj);
    }
    void drawNode(const gltf::Node &node, glm::mat4 mat)
    {
      if (node.mesh > -1)
      {
        assert(node.mesh < model.meshes.size());

        // skinning
        if (node.skin > -1)
        {
          const gltf::Skin &skin = model.skins[node.skin];

          glm::mat4 nodeInverse = glm::inverse(mat);
          std::vector<glm::mat4> jointMatrices = std::vector<glm::mat4>(skin.joints.size());
          for (uint i = 0; i < skin.joints.size(); i++)
          {
            jointMatrices[i] = nodeInverse * skiningMatrices[skin.joints[i]];
          }
          shader.setMat4Arr("u_jointMatrix", jointMatrices.size(), (float *)jointMatrices.data());
        }

        const gltf::Mesh &mesh = model.meshes[node.mesh];
        glBindVertexArray(gltfMeshVAO[node.mesh]);
        shader.setMat4("node", mat);
        uint sampler_obj;
        glGenSamplers(1, &sampler_obj);
        for (uint i = 0; i < mesh.primitives.size(); i++)
        {
          const gltf::Mesh::Primitive &primitive = mesh.primitives[i];
          const gltf::Accessor &indexAccessor =
              model.accessors[primitive.indices];
          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gltfBufferViewVBO[indexAccessor.bufferView]);

          // material rendering
          bool hasBaseColorTexture = 0;
          bool doubleSided = false;
          if (primitive.material > -1)
          {
            const gltf::Material &material = model.materials[primitive.material];
            if (material.alphaMode != current_alphaMode)
            {
              continue;
            }
            uint texCoord = 0;
            bool KHR_materials_unlit = vrmData != NULL || gltf::findExtensionIndex("KHR_materials_unlit", material) != -1;
            
            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
              texCoord = material.pbrMetallicRoughness.baseColorTexture.texCoord;
              hasBaseColorTexture = 1;
              const gltf::Texture &texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
              bindTexture(texture, texCoord, sampler_obj, 0);
            }
            if (material.pbrMetallicRoughness.baseColorFactor.size() > 0)
            {
              glm::vec4 baseColorFactor;
              memcpy(&baseColorFactor, material.pbrMetallicRoughness.baseColorFactor.data(), sizeof(float) * 4);
              shader.setVec4("baseColorFactor", baseColorFactor);
            }
            if (material.emissiveTexture.index >= 0 && KHR_materials_unlit)
            {
              texCoord = material.emissiveTexture.texCoord;
              hasBaseColorTexture = 1;
              const gltf::Texture &texture = model.textures[material.emissiveTexture.index];
              bindTexture(texture, texCoord, sampler_obj, 2);
            }
            if (material.normalTexture.index >= 0)
            {
              texCoord = material.normalTexture.texCoord;
              const gltf::Texture &texture = model.textures[material.normalTexture.index];
              bindTexture(texture, texCoord, sampler_obj, 1);
            }

            shader.setBool("KHR_materials_unlit", KHR_materials_unlit);
            if (material.alphaMode == gltf::Material::OPAQUE)
            {
              glDisable(GL_BLEND);
            }
            else if (material.alphaMode == gltf::Material::MASK)
            {
              glDisable(GL_BLEND);
            }
            else if (material.alphaMode == gltf::Material::BLEND)
            {
              glEnable(GL_BLEND);
            }
            doubleSided = material.doubleSided;

            shader.setInt("texCoordIndex", texCoord);

            if (vrmData)
            {
              // yes, for some reason it has both material.alphaCutoff and this. My testing seems to indicate they have the same value tho
              gltf::Extensions::VRM::MaterialProperties *materialProperties = vrmMaterialProperties[primitive.material];
              shader.setFloat("alphaCutoff", materialProperties->floatProperties._Cutoff);
              if (shaderType == ShaderType::mtoon)
              {
                shader.setFloat("shadeToony", materialProperties->floatProperties._ShadeToony);
                shader.setFloat("shadeShift", materialProperties->floatProperties._ShadeShift);
                shader.setVec4("materialColor", materialProperties->vectorProperties._Color.glm_vec);
                shader.setVec4("shadeColor", materialProperties->vectorProperties._ShadeColor.glm_vec);


  // enum MToonMaterialOutlineWidthMode {
  //   None,
  //   WorldCoordinates,
  //   ScreenCoordinates,
  // }
                if (materialProperties->floatProperties._OutlineWidthMode != 0)
                {
                  glEnable(GL_CULL_FACE);
                  glEnable(GL_LINE_SMOOTH);
                  glCullFace(GL_FRONT);
                  glFrontFace(GL_CCW);
                  shader.setFloat("outlineWidth", materialProperties->floatProperties._OutlineWidth);
                  if (materialProperties->vectorProperties._OutlineColor.w <= 0.001)
                  {
                    materialProperties->vectorProperties._OutlineColor.w = 1;
                  }
                  shader.setVec4("outlineColor", materialProperties->vectorProperties._OutlineColor.glm_vec);
                  if(materialProperties->floatProperties._OutlineColorMode>0){
                    shader.setFloat("outlineMix", materialProperties->floatProperties._OutlineLightingMix);
                  }else{
                    shader.setFloat("outlineMix", 0);
                  }
                  glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                                reinterpret_cast<void *>(indexAccessor.byteOffset));

                  glCullFace(GL_BACK);
                  glFrontFace(GL_CCW);
                  shader.setFloat("outlineWidth", 0.f);
                }

              }
            }
            else
            {
              shader.setFloat("alphaCutoff", material.alphaCutoff);
            }


          }
          else
          {
            // so that we only render the primitive on the first pass
            if (current_alphaMode != 0)
            {
              continue;
            }
          }
          shader.setBool("hasBaseColorTexture", hasBaseColorTexture);

          if (doubleSided)
          {
            glDisable(GL_CULL_FACE);
          }
          else
          {
            glEnable(GL_CULL_FACE);
          }

          glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                         reinterpret_cast<void *>(indexAccessor.byteOffset));
        }

        glDeleteSamplers(1, &sampler_obj);
      }

      for (size_t i = 0; i < node.children.size(); i++)
      {
        assert(node.children[i] < model.nodes.size());
        drawNode(model.nodes[node.children[i]], nodeTransforms[node.children[i]]);
      }
    }
  };

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
  }

  static struct
  {
    float lastX = SCREEN_WIDTH / 2.0f;
    float lastY = SCREEN_HEIGHT / 2.0f;
    bool firstMouse = true;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int realWindowWidth = SCREEN_WIDTH, realWindowHeight = SCREEN_HEIGHT;
  } windowData;
  static VModel vmodel;

  static void launch()
  {
    if (!glfwInit())
    {
      assert(0 && "could not create context");
      return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test", NULL, NULL);
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
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glfwGetWindowSize(window, &windowData.realWindowWidth, &windowData.realWindowHeight);

    // init
    if (1)
    {
      vmodel.loadModel(VMODEL);
      // vmodel.loadModel(VMODEL, Filetype::gltf);

      vmodel.animate(0);

      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                               {
                                   if (windowData.firstMouse)
                                   {
                                     windowData.lastX = xpos;
                                     windowData.lastY = ypos;
                                     windowData.firstMouse = false;
                                   }

                                   float xoffset = xpos - windowData.lastX;
                                   float yoffset = windowData.lastY - ypos; // reversed since y-coordinates go from bottom to top

                                   windowData.lastX = xpos;
                                   windowData.lastY = ypos;

                                   vmodel.camera.ProcessMouseMovement(xoffset, yoffset); });
    }

    while (!glfwWindowShouldClose(window))
    {
      glfwMakeContextCurrent(window);

      float currentFrame = glfwGetTime();
      windowData.deltaTime = currentFrame - windowData.lastFrame;
      windowData.lastFrame = currentFrame;

      glClearColor(0.3f, 0.3f, 0.3f, 1.f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      Shader &shader = vmodel.shader;
      shader.use();

      glm::mat4 projection = glm::perspective(glm::radians(vmodel.camera.Zoom), (float)windowData.realWindowWidth / (float)windowData.realWindowHeight, 0.1f, 100.0f);
      glm::mat4 view = vmodel.camera.GetViewMatrix();
      shader.setMat4("projection", projection);
      shader.setMat4("view", view);
      shader.setVec3("viewPos",vmodel.camera.Position);

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
      model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * ((float)01));
      shader.setMat4("model", model);
      struct Light
      {
        glm::vec3 Position;
        glm::vec3 Color;
        float Intensity;
      } lights = {glm::vec3(10, 0, -10), glm::vec3(1, 1, 1), 0.2};
      shader.setVec3("lights[0].Position", lights.Position);
      shader.setVec3("lights[0].Color", lights.Color);
      shader.setFloat("lights[0].Intensity", lights.Intensity);

      vmodel.draw();

      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        vmodel.camera.ProcessKeyboard(FORWARD, windowData.deltaTime);
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        vmodel.camera.ProcessKeyboard(BACKWARD, windowData.deltaTime);
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        vmodel.camera.ProcessKeyboard(LEFT, windowData.deltaTime);
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        vmodel.camera.ProcessKeyboard(RIGHT, windowData.deltaTime);
      // q(A),e,i,o,u, b(blink)
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        vmodel.setExpression("a");
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        vmodel.setExpression("e");
      if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        vmodel.setExpression("i");
      if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        vmodel.setExpression("o");
      if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
        vmodel.setExpression("u");
      if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        vmodel.setExpression("b");
      glfwSwapInterval(1); // v-sync
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

  static void start(std::function<void(GLFWwindow *)> init, std::function<void(GLFWwindow *)> draw)
  {
    if (!glfwInit())
    {
      assert(0 && "could not create context");
      return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "test", NULL, NULL);
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
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    init(window);

    while (!glfwWindowShouldClose(window))
    {
      glfwMakeContextCurrent(window);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      draw(window);

      glfwSwapInterval(1); // v-sync
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

}
