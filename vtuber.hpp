#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/core.hpp>
// #include <opencv2/gapi.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "vrmLoader.hpp"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <functional>
#include <fstream>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#ifndef VMODEL
#define VMODEL "models/male1.glb"
#endif
#define VERTEX_SHADER "shaders/vertex_shader.glsl"
#define FRAGMENT_SHADER "shaders/fragment_shader.glsl"

#define convertVec2(dest, src) dest.x=src.x;dest.y=src.y;
#define convertVec3(dest, src) convertVec2(dest, src)dest.z=src.z;
#define convertVec4(dest, src) convertVec3(dest, src)dest.w=src.w;

namespace vtuber
{
  typedef unsigned char uchar;
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
    Array(L l)
    {
      length = l;
      arr = new T[l];
    }
    Array(L l, T arr)
    {
      this->length = l;
      this->arr = arr;
    }
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
  static inline float randf()
  {
    return ((float)rand()) / RAND_MAX;
  }

  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
  };
  struct Texture_gl
  {
    uint id;
    std::string type;
  };
  struct Mesh
  {
  public:
    std::vector<Vertex> vertices;
    std::vector<uint> indices;
    std::vector<Texture_gl> textures;
    uint VAO, VBO, EBO;

    Mesh(std::vector<Vertex> vertices,std::vector<uint> indices,std::vector<Texture_gl> textures)
    {
      this->vertices = vertices;
      this->indices = indices;
      this->textures = textures;

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

      // vertex positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
      // vertex normals
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
      // vertex texture coords
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));
      // vertex tangent
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));
      // vertex bitangent
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

      glBindVertexArray(0);
    }
  };
  class Shader
  {
  public:
    uint ID;
    Shader():ID(0){
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
    glTF::glTFModel model;
    Array<uint> gltfBufferViewVBO;
    Array<Array<uint>>gltfMeshPrimitiveVAO;
    Array<uint>gltfImageTextureIndex;

    VModel()
    {
    }
    VModel(std::string path)
    {
      loadModel(path);
    }
    VModel(glTF::glTFModel model)
    {
      this->model = model;
    }
    void loadModel(std::string path)
    {
      Importer importer;
      importer.import(path);
      model = importer.model;

      gltfBufferViewVBO = Array<uint>(model.buffers.size());
      for (uint i = 0; i < model.buffers.size(); i++)
      {
        const glTF::BufferView &bufferView = model.bufferViews[i];

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

        const glTF::Buffer &buffer = model.buffers[bufferView.buffer];
        uint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(bufferView.target, VBO);

        if (sparse_accessor < 0)
          glBufferData(bufferView.target, bufferView.byteLength,
                       buffer.buffer + bufferView.byteOffset,
                       GL_STATIC_DRAW);
        else
        {
          const auto& accessor = model.accessors[sparse_accessor];
          // copy the buffer to a temporary one for sparse patching
          uchar *tmp_buffer = new uchar[bufferView.byteLength];
          memcpy(tmp_buffer, buffer.buffer + bufferView.byteOffset,
                 bufferView.byteLength);

          const uint size_of_object_in_buffer =
              glTF::gltf_sizeof(accessor.componentType);
          const uint size_of_sparse_indices =
              glTF::gltf_sizeof(accessor.sparse.indices.componentType);

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
            }
            const uchar *read_from =
                values_buffer.buffer +
                (values_buffer_view.byteOffset +
                 accessor.sparse.values.byteOffset) +
                (sparse_index * (size_of_object_in_buffer * glTF::gltf_num_components(accessor.type)));

            uchar *write_to =
                tmp_buffer + index * (size_of_object_in_buffer * glTF::gltf_num_components(accessor.type));

            memcpy(write_to, read_from, size_of_object_in_buffer * glTF::gltf_num_components(accessor.type));
          }

          // debug:
          /*for(size_t p = 0; p < bufferView.byteLength/sizeof(float); p++)
        {
          float* b = (float*)tmp_buffer;
          std::cout << "modified_buffer [" << p << "] = " << b[p] << '\n';
        }*/

          glBufferData(bufferView.target, bufferView.byteLength, tmp_buffer,
                       GL_STATIC_DRAW);
          delete[] tmp_buffer;
        }
        glBindBuffer(bufferView.target, 0);

        gltfBufferViewVBO[i] = VBO;
      }
    }
    void draw(Shader &shader)
    {
    }
  };

#define vmodeltest
#ifdef vmodeltest
#define VModel VModel2
  class VModel
  {
  public:
    std::vector<Mesh> meshes;

    VModel()
    {
    }
    VModel(std::string path)
    {
      loadModel(path);
    }

    void loadModel(std::string path){
      Assimp::Importer import;
      const aiScene *scene = import.ReadFile(path,
      aiProcess_Triangulate |
      aiProcess_GenSmoothNormals |
      aiProcess_FlipUVs |
      aiProcess_CalcTangentSpace |
      aiProcess_EmbedTextures|
      aiProcess_SortByPType);

      if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
      }

      processNode(scene->mRootNode, scene);
    }

    void draw(Shader&shader){
      for (uint i = 0; i < meshes.size(); i++)
      {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (uint j = 0; j < meshes[i].textures.size(); j++)
        {
          glActiveTexture(GL_TEXTURE0 + j);
          std::string number;
          std::string name = meshes[i].textures[j].type;
          if (name == "texture_diffuse"){
            number = std::to_string(diffuseNr++);
          }
          else if (name == "texture_specular"){
            number = std::to_string(specularNr++);
          }
          else if (name == "texture_normal"){
            number = std::to_string(normalNr++);
          }
          else if (name == "texture_height"){
            number = std::to_string(heightNr++);
          }

          // glUniform1i(glGetUniformLocation(shader.ID, "texture"), i);
          // std::cout<<(name + number)<<std::endl;
          glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), j);
          glBindTexture(GL_TEXTURE_2D, meshes[i].textures[j].id);

//--------------------------------------

          // glActiveTexture(GL_TEXTURE0 + j); // active proper texture unit before binding

          // // now set the sampler to the correct texture unit
          // glUniform1i(glGetUniformLocation(shader.ID, "texture"), j);
          // glBindTexture(GL_TEXTURE_2D, meshes[i].textures[j].id);
        }
        glBindVertexArray(meshes[i].VAO);
        glDrawElements(GL_TRIANGLES, meshes[i].indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
      }
    }
  private:
    std::vector<Texture_gl> loaded_textures;
    void processNode(aiNode *node, const aiScene *scene)
    {
      for (uint i = 0; i < node->mNumMeshes; i++)
      {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
      }
      for (unsigned int i = 0; i < node->mNumChildren; i++)
      {
        processNode(node->mChildren[i], scene);
      }
    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
      std::vector<Vertex> vertices;
      std::vector<uint> indices;
      std::vector<Texture_gl> textures;
      for (uint i = 0; i < mesh->mNumVertices; i++)
      {
        Vertex vertex_;
        glm::vec3 vector;
        convertVec3(vector, mesh->mVertices[i]);
        vertex_.position = vector;
        if (mesh->HasNormals())
        {
          convertVec3(vector, mesh->mNormals[i]);
          vertex_.normal = vector;
        }
        if (mesh->mTextureCoords[0])
        {
          glm::vec2 vec;
          convertVec2(vec, mesh->mTextureCoords[0][i]);
          vertex_.texCoords = vec;
          convertVec3(vector, mesh->mTangents[i]);
          vertex_.tangent = vector;
          convertVec3(vector, mesh->mBitangents[i]);
          vertex_.bitangent = vector;
        }
        else
        {
          vertex_.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex_);
      }
      for (uint i = 0; i < mesh->mNumFaces; i++)
      {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++)
        {
          indices.push_back(face.mIndices[j]);
        }
      }
      aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      std::vector<Texture_gl> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, scene);
      textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
      std::vector<Texture_gl> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, scene);
      textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
      std::vector<Texture_gl> normalMaps = loadMaterialTexture(material, aiTextureType_HEIGHT, scene);
      textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
      std::vector<Texture_gl> heightMaps = loadMaterialTexture(material, aiTextureType_AMBIENT, scene);
      textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
      return Mesh(vertices, indices, textures);
    }
    std::vector<Texture_gl> loadMaterialTexture(const aiMaterial *material,const aiTextureType type, const aiScene *scene)
    {
      std::vector<Texture_gl>textures;
      for (uint i = 0; i < material->GetTextureCount(type);i++)
      {
        aiString texture_file;
        material->Get(AI_MATKEY_TEXTURE(type, i), texture_file);
        Texture_gl tex;
        bool included = false;
        for (uint j = 0; j < loaded_textures.size(); j++)
        {
          if (loaded_textures[j].type == std::string(texture_file.C_Str()))
          {
            tex.id = loaded_textures[j].id;
            included = true;
            break;
          }
        }
        if(!included)
        {
          uint textureID;
          glGenTextures(1, &textureID);
          glBindTexture(GL_TEXTURE_2D, textureID);
          if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str()))
          {
            if (texture->mHeight > 0)
            {
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texture->mWidth, texture->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                          texture->pcData);
              glGenerateMipmap(GL_TEXTURE_2D);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (texture->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (texture->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            }
            else
            {
              // if(std::string(texture->achFormatHint)=="png")
              // {
                cv::Mat image(1, texture->mWidth, CV_8UC1, (void *)texture->pcData);
                image = cv::imdecode(image, cv::IMREAD_ANYCOLOR);

                // std::cout<<image.channels()<<std::endl;
                // cv::Mat shown=image.clone();
                // cv::resize(shown, shown, cv::Size(), 0.25, 0.25);
                // cv::imshow(texture_file.C_Str(), shown);

                cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
                // cv::flip(image, decodedImage, 0);

                if (!image.empty())
                {
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());
                }
                else
                {
                  std::cout << "Could not decode embeded texture '" << texture_file.C_Str() << "'" << std::endl;
                }
              // }
            }
          }
          else
          {
            cv::Mat image = cv::imread(std::string("models/") + texture_file.C_Str(), cv::IMREAD_UNCHANGED | cv::IMREAD_ANYCOLOR | cv::IMREAD_IGNORE_ORIENTATION);
            cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
            cv::flip(image, image, 0);

            if (!image.empty())
            {
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
              glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
              glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.ptr());
              glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
              std::cout << "Could not find embeded png texture '" << texture_file.C_Str() << "'" << std::endl;
            }
          }

          tex.id = textureID;
          tex.type = texture_file.C_Str();
          loaded_textures.push_back(tex);
        }
        switch (type)
        {
        case aiTextureType_DIFFUSE:
          tex.type = "texture_diffuse";
          break;
        case aiTextureType_SPECULAR:
          tex.type = "texture_specular";
          break;
        case aiTextureType_HEIGHT:
          tex.type = "texture_normal";
          break;
        case aiTextureType_AMBIENT:
          tex.type = "texture_height";
          break;
        default:
          break;
        }
        textures.push_back(tex);
      }
      return textures;
    }
  };
#endif
  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
  }

  void launch(std::function<void(GLFWwindow *)> init, std::function<void(GLFWwindow*)> draw)
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