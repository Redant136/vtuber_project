#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <opencv2/core.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define CHEVAN_UTILS_incl
#define CHEVAN_UTILS_B64
#define CHEVAN_UTILS_VEC2 glm::vec2
#define CHEVAN_UTILS_VEC3 glm::vec3
#define CHEVAN_UTILS_VEC4 glm::vec4
#define CHEVAN_UTILS_PRINT
#include "utils.hpp"
namespace chevan_utils
{
  void println(glm::mat4 mat)
  {
    printMat4((float *)&mat);
  }
};
using namespace chevan_utils;
#define print println

#include "vrmLoader.hpp"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#ifndef VMODEL
#define VMODEL "models/male1.glb"
#endif
#define VERTEX_SHADER "shaders/vertex_shader.vert"
#define FRAGMENT_SHADER "shaders/fragment_shader.frag"

namespace vtuber
{
  class Shader
  {
  public:
    uint ID;
    Shader() : ID(0)
    {
    }
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr)
    {
      // 1. retrieve the vertex/fragment source code from filePath
      std::string vertexCode;
      std::string fragmentCode;
      std::string geometryCode;
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

  class VModel
  {
  public:
    gltf::glTFModel model;
    Array<uchar *> gltfBuffers;
    Array<uint> gltfBufferViewVBO;
    Array<uint> gltfMeshVAO;
    Array<uint> gltfImageTextureIndex;
    Array<bool> updatedNodeMorphs;
    Array<glm::mat4> nodeTransforms;

    struct
    {
      uint animation = -1;
      float animationStartTime = 0.f;
    } animationData;

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
      glDeleteVertexArrays(gltfMeshVAO.length, gltfMeshVAO.arr);
      glDeleteBuffers(gltfBufferViewVBO.length, gltfBufferViewVBO.arr);
      glDeleteTextures(gltfImageTextureIndex.length, gltfImageTextureIndex.arr);
      gltfMeshVAO.free();
      gltfBufferViewVBO.free();
      gltfImageTextureIndex.free();
      gltfBuffers.free();
    }

    void loadModel(std::string path, const Filetype type = Filetype::vrm)
    {
      Importer importer;
      importer.import(path, type);
      model = importer.model;

      // buffers
      gltfBuffers = Array<uchar *>(model.buffers.size());
      for (uint i = 0; i < model.buffers.size(); i++)
      {
        const gltf::Buffer &buffer = model.buffers[i];
        gltfBuffers[i] = new uchar[buffer.byteLength];
        memcpy(gltfBuffers[i], buffer.buffer, buffer.byteLength);
      }

      // setup morph weights and matrix
      updatedNodeMorphs = Array<bool>(model.nodes.size());
      for (uint i = 0; i < model.nodes.size(); i++)
      {
        updatedNodeMorphs[i] = true;
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
            node.weights[j] = 1;
          }
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

      update();

      // VAOs
      gltfMeshVAO = Array<uint>(model.meshes.size());
      for (uint i = 0; i < model.meshes.size(); i++)
      {
        const gltf::Mesh &mesh = model.meshes[i];
        uint VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        for (uint j = 0; j < mesh.primitives.size(); j++)
        {
          const gltf::Mesh::Primitive &primitive = mesh.primitives[j];

          struct
          {
            std::string accName;
            int attribIndex;
          } attribs[] = {{"POSITION", 0}, {"NORMAL", 1}, {"TEXCOORD", 2}, {"JOINTS", 3}, {"WEIGHTS", 4}};

          for (uint k = 0; k < sizeof(attribs) / sizeof(attribs[0]); k++)
          {
            if (gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName, 0) == -1)
              continue;
            const gltf::Accessor &accessor = model.accessors[gltf::getMeshPrimitiveAttribVal(primitive.attributes, attribs[k].accName, 0)];
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
                                  byteStride, reinterpret_cast<void*>(accessor.byteOffset));
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
        uchar *im;
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

    }
    void drawSkeleton(Shader &shader)
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
    void draw(Shader &shader)
    {
      update();

      const gltf::Scene &scene = model.scenes[model.scene > -1 ? model.scene : 0];
      for (uint i : scene.nodes)
      {
        drawNode(shader, model.nodes[i], nodeTransforms[i]);
      }
      // drawSkeleton(shader);
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
  private:
    static glm::mat4 getNodeTRS(const gltf::Node&node)
    {
      glm::mat4 t = glm::mat4(1.f);
      glm::mat4 r = glm::mat4(1.f);
      glm::mat4 s = glm::mat4(1.f);

      if (node.scale.size() != 0)
      {
        t = glm::scale(glm::mat4(1.f), glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
      }
      if (node.rotation.size() != 0)
      {
        r = glm::mat4_cast(glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]));
      }
      if (node.translation.size() != 0)
      {
        s = glm::translate(glm::mat4(1.f), glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
      }
      return t*r*s;
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
            weights[i] *= mesh.weights[i];
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
                glNamedBufferSubData(gltfBufferViewVBO[accessor.bufferView], accessor.byteOffset + (byteStride)*j, sizeof(float) * 3, &position);
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
      glm::mat4 mat = glm::mat4(1.f);
      mat = parentTransform * mat;
      if (model.nodes[node].matrix.size() != 0)
        mat *= glm::make_mat4((float*)model.nodes[node].matrix.data());
      mat *= getNodeTRS(model.nodes[node]);
      nodeTransforms[node] = mat;

      for (uint i : model.nodes[node].children)
      {
        model.nodes[i].parentNode = node;
        updateNodeMatrix(i, mat);
      }
    }

    void drawNode(Shader &shader, const gltf::Node &node, glm::mat4 mat)
    {
      if (node.mesh > -1)
      {
        assert(node.mesh < model.meshes.size());

        bool test = 0;
        // skinning
        // TODO(ANT) fix skinning
        if (!test && node.skin > -1)
        {
          const gltf::Skin &skin = model.skins[node.skin];
          
          glm::mat4 nodeInverse = glm::inverse(mat);

          std::vector<glm::mat4> jointMatrices = std::vector<glm::mat4>(skin.joints.size());
          for (uint i = 0; i < skin.joints.size(); i++)
          {
            glm::mat4 jointNodeMat = nodeTransforms[skin.joints[i]];
            glm::mat4 inverseBindMatrix = glm::make_mat4((float *)gltf::getDataFromAccessor(model, model.accessors[skin.inverseBindMatrices], i));


            // TODO(ANT)
            glm::mat4 jointMatrix = glm::mat4(1.f);
            jointMatrix *= nodeInverse;
            jointMatrix *= jointNodeMat;
            jointMatrix *= inverseBindMatrix;

            jointMatrices[i] = jointMatrix;
            print(jointMatrices[i]);
            printSep();
          }
          glUniformMatrix4fv(glGetUniformLocation(shader.ID, "u_jointMatrix"), jointMatrices.size(), GL_FALSE, (float *)jointMatrices.data());
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
          if (primitive.material >= 0)
          {
            const gltf::Material &material = model.materials[primitive.material];
            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
              const gltf::Texture &texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];

              const gltf::Sampler &sampler = model.samplers[texture.sampler];
              glSamplerParameterf(sampler_obj, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
              glSamplerParameterf(sampler_obj, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
              glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_S, sampler.wrapS);
              glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_T, sampler.wrapT);

              glActiveTexture(GL_TEXTURE0);

              glBindTexture(GL_TEXTURE_2D, gltfImageTextureIndex[texture.source]);
              glBindSampler(GL_TEXTURE_2D, sampler_obj);
            }
            if (material.pbrMetallicRoughness.baseColorFactor.size() > 0)
            {
              glm::vec4 baseColorFactor;
              memcpy(&baseColorFactor, material.pbrMetallicRoughness.baseColorFactor.data(), sizeof(float) * 4);
              shader.setVec4("baseColorFactor", baseColorFactor);
            }
            if (material.emissiveTexture.index >= 0)
            {
              const gltf::Texture &texture = model.textures[material.emissiveTexture.index];

              const gltf::Sampler &sampler = model.samplers[texture.sampler];
              glSamplerParameterf(sampler_obj, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
              glSamplerParameterf(sampler_obj, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
              glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_S, sampler.wrapS);
              glTexParameteri(sampler_obj, GL_TEXTURE_WRAP_T, sampler.wrapT);

              glActiveTexture(GL_TEXTURE4);

              glBindTexture(GL_TEXTURE_2D, gltfImageTextureIndex[texture.source]);
              glBindSampler(GL_TEXTURE_2D, sampler_obj);
            }
          }

          glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
                         reinterpret_cast<void *>(indexAccessor.byteOffset));
        }

        glDeleteSamplers(1, &sampler_obj);
      }

      for (size_t i = 0; i < node.children.size(); i++)
      {
        assert(node.children[i] < model.nodes.size());
        drawNode(shader, model.nodes[node.children[i]], mat);
      }
    }
  };

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
  }

  void launch(std::function<void(GLFWwindow *)> init, std::function<void(GLFWwindow *)> draw)
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
    glEnable(GL_DEPTH_TEST);

    init(window);

    while (!glfwWindowShouldClose(window))
    {
      glfwMakeContextCurrent(window);
      glClear(GL_COLOR_BUFFER_BIT);

      draw(window);
      glfwSwapInterval(1); //v-sync
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

}