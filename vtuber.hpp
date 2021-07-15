#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <opencv2/core.hpp>
#include <opencv2/gapi.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>

#include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <assert.h>

#define WIDTH 1920
#define HEIGHT 1080
#define VMODEL "models/male1.glb"

#define convertVec2(dest, src) dest.x=src.x;dest.y=src.y;
#define convertVec3(dest, src) convertVec2(dest, src)dest.z=src.z;
#define convertVec4(dest, src) convertVec3(dest, src)dest.w=src.w;

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
  static inline float randf()
  {
    return ((float)rand()) / RAND_MAX;
  }

  struct vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
  };
  struct texture_gl
  {
    uint id;
    std::string type;
  };
  struct Mesh
  {
  public:
    std::vector<vertex> vertices;
    std::vector<uint> indices;
    std::vector<texture_gl> textures;
    uint VAO, VBO, EBO;

    Mesh(std::vector<vertex> vertices,std::vector<uint> indices,std::vector<texture_gl> textures)
    {
      this->vertices = vertices;
      this->indices = indices;
      this->textures = textures;

      glGenVertexArrays(1, &VAO);
      glGenBuffers(1, &VBO);
      glGenBuffers(1, &EBO);

      glBindVertexArray(VAO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO);

      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                   &indices[0], GL_STATIC_DRAW);

      // vertex positions
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
      // vertex normals
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *) sizeof(glm::vec3));
      // vertex texture coords
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3)+sizeof(glm::vec3)));

      glBindVertexArray(0);
    }
  };
  
  class VModel{
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
      const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

      if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
      {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
      }

      processNode(scene->mRootNode, scene);
    }

  private:
    std::vector<texture_gl> loaded_textures;
  
    void processNode(aiNode *node, const aiScene *scene)
    {
      for (uint i = 0; i < node->mNumMeshes; i++)
      {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
      }
      // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
      for (unsigned int i = 0; i < node->mNumChildren; i++)
      {
        processNode(node->mChildren[i], scene);
      }
    }
    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {
      std::vector<vertex> vertices;
      std::vector<uint> indices;
      std::vector<texture_gl> textures;

      for (uint i = 0; i < mesh->mNumVertices; i++)
      {
        vertex vertex_;
        glm::vec3 vector;
        convertVec3(vector, mesh->mVertices[i]);
        vertex_.position=vector;
        convertVec3(vector, mesh->mNormals[i]);
        vertex_.normal=vector;
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
          glm::vec2 vec;
          convertVec2(vec,mesh->mTextureCoords[0][i]);
          vertex_.texCoords = vec;
        }
        else
          vertex_.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex_);
      }
      for (uint i = 0; i < mesh->mNumFaces; i++)
      {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++){
          indices.push_back(face.mIndices[j]);
        }
      }
      if (mesh->mMaterialIndex >= 0)
      {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<texture_gl> diffuseMaps = loadMaterialTexture(material, aiTextureType_DIFFUSE, scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<texture_gl> specularMaps = loadMaterialTexture(material, aiTextureType_SPECULAR, scene);
        textures.insert(textures.end(), specularMaps.begin(),specularMaps.end());
      }

      return Mesh(vertices, indices, textures);
    }
    std::vector<texture_gl> loadMaterialTexture(const aiMaterial *material,const aiTextureType type, const aiScene *scene)
    {
      std::vector<texture_gl>textures;
      // for(uint i=0;i<scene->mNumTextures;i++){
      //   std::cout<<scene->mTextures[i]->mFilename.C_Str()<<std::endl;
      // }
      for (uint i = 0; i < material->GetTextureCount(type);i++){
        aiString texture_file;
        material->Get(AI_MATKEY_TEXTURE(type, i), texture_file);
        bool included=false;
        for(uint j=0;j<loaded_textures.size();j++){
          if(loaded_textures[i].type==texture_file.C_Str()){
            textures.push_back(loaded_textures[j]);
            included=true;
            break;
          }
        }
        if(!included){
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
              if(std::string(texture->achFormatHint)=="png"){
                // Mat a;
                cv::Mat img = cv::imread("path_to_image", cv::IMREAD_ANYCOLOR);
//TODO HERE
                // imshow("img", img);
              }
              // std::cout << "Could not find texture '" << texture_file.C_Str() << "'" << std::endl;
            }
          }
          else
          {
            std::cout<<"not embedded texture"<<std::endl;
          }
          texture_gl tex={textureID,texture_file.C_Str()};
          
          loaded_textures.push_back(tex);
          textures.push_back(tex);
        }
      }
      return textures;
    }
  };

  static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
  {
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
  }

  static void compileShaders()
  {
  }
  static void init()
  {
    VModel model=VModel(VMODEL);
  }
  static void draw()
  {
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
    compileShaders();
    init();

    while (!glfwWindowShouldClose(window))
    {
      glfwMakeContextCurrent(window);
      glClear(GL_COLOR_BUFFER_BIT);


      // draw here
      draw();
      glfwSwapInterval(1); //v-sync
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  }

}