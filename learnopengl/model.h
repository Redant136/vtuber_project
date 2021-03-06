#ifndef MODEL_H
#define MODEL_H
#include <opencv2/core.hpp>
#include <opencv2/gapi.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
// #include <opencv2/core/opengl.hpp>

#include <glad/glad.h>
#include <gl/GL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory,const aiScene* scene, bool gamma = false);

class Model
{
public:
  // model data
  vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
  vector<Mesh> meshes;
  string directory;
  bool gammaCorrection;

  // constructor, expects a filepath to a 3D model.
  Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
  {
    loadModel(path);
  }

  // draws the model, and thus all its meshes
  void Draw(Shader &shader)
  {
    for (unsigned int i = 0; i < meshes.size(); i++)
      meshes[i].Draw(shader);
  }

private:
  // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
  void loadModel(string const &path)
  {
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
      cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
      return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
  }

  // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
  void processNode(aiNode *node, const aiScene *scene)
  {
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
      // the node object only contains indices to index the actual objects in the scene.
      // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
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
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
      Vertex vertex;
      glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
      // positions
      vector.x = mesh->mVertices[i].x;
      vector.y = mesh->mVertices[i].y;
      vector.z = mesh->mVertices[i].z;
      vertex.Position = vector;
      // normals
      if (mesh->HasNormals())
      {
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
      }
      // texture coordinates
      if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
      {
        glm::vec2 vec;
        // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
        // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
        vec.x = mesh->mTextureCoords[0][i].x;
        vec.y = mesh->mTextureCoords[0][i].y;
        vertex.TexCoords = vec;
        // tangent
        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;
        // bitangent
        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;
      }
      else
        vertex.TexCoords = glm::vec2(0.0f, 0.0f);

      vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
      aiFace face = mesh->mFaces[i];
      // retrieve all indices of the face and store them in the indices vector
      for (unsigned int j = 0; j < face.mNumIndices; j++)
        indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",scene);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // // 2. specular maps
    // vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular",scene);
    // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // // 3. normal maps
    // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal",scene);
    // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // // 4. height maps
    // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height",scene);
    // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
  }

  // checks all material textures of a given type and loads the textures if they're not loaded yet.
  // the required info is returned as a Texture struct.
  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName,const aiScene* scene)
  {
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
      aiString str;
      mat->GetTexture(type, i, &str);
      // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
      bool skip = false;
      for (unsigned int j = 0; j < textures_loaded.size(); j++)
      {
        if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
        {
          textures.push_back(textures_loaded[j]);
          skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
          break;
        }
      }
      if (!skip)
      { // if texture hasn't been loaded already, load it
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), this->directory,scene);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
        textures_loaded.push_back(texture); // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
      }
    }
    return textures;
  }
};

unsigned int TextureFromFile(const char *path, const string &directory,const aiScene* scene, bool gamma)
{
  string filename = string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  if ('*' == path[0])
  {
    unsigned int index=std::stoi(std::string(path).substr(1, std::string(path).length()));
    // std::cout << std::stoi(std::string(path).substr(1, std::string(path).length())) << std::endl;
    // const aiTexture *tex = scene->mTextures[std::stoi(std::string(path).substr(1,std::string(path).length()))];

    const aiTexture *tex = scene->GetEmbeddedTexture(path);

    // aiMaterial* material = scene->mMaterials[index];
    // aiString texture_file;
    // material->Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file);
    // const aiTexture* tex=scene->GetEmbeddedTexture(texture_file.C_Str());

    // for (int i = 0; i < scene->mNumTextures; i++)
    // {
    //   std::cout << scene->mTextures[i]->mWidth << std::endl;
    //   std::cout << scene->mTextures[i]->mHeight << std::endl;
    // }

    // if (true)
    if (tex->mHeight > 0)
    {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->mWidth, tex->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                   tex->pcData);

      glGenerateMipmap(GL_TEXTURE_2D);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (tex->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (tex->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
      // if (std::string(tex->achFormatHint) == "png")
      // {
        cv::Mat rawData(1, tex->mWidth, CV_8UC1, (void *)tex->pcData);
        cv::Mat decodedImage = cv::imdecode(rawData, cv::IMREAD_ANYCOLOR);
        // cv::imshow("img",decodedImage);
        if (decodedImage.data)
        {
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, decodedImage.cols, decodedImage.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, decodedImage.ptr());
        }
        else
        {
          std::cout << "Could not find embeded png texture '" << path << "'" << std::endl;
        }
      // }
    }
  }
  else
  {
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
      // cv::Mat stbImg=cv::Mat(1, nSize, CV_8UC1, (void*)pcBuffer );
      GLenum format;
      if (nrComponents == 1){
        format = GL_RED;  
      }
      else if (nrComponents == 3){
        format = GL_RGB;
      }
      else if (nrComponents == 4){
        format = GL_RGBA;
      }

      glBindTexture(GL_TEXTURE_2D, textureID);
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      // std::cout << filename << std::endl;
      // cv::Mat opencvImg = cv::imread(filename);
      // std::cout<<nrComponents<<std::endl;
      // std::cout<<opencvImg.channels()<<std::endl;
      // cv::imshow("stb", cv::Mat(width, height, CV_8UC3, data));
      // cv::imshow("opencv", cv::imread(filename.c_str(), cv::IMREAD_UNCHANGED));

      stbi_image_free(data);
    }else{
      std::cout << "Texture failed to load at path: '" << path << "'" << std::endl;
      stbi_image_free(data);
    }

    // cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED|cv::IMREAD_COLOR|cv::IMREAD_IGNORE_ORIENTATION);
    
    // cv::cvtColor(image, image, cv::COLOR_BGR2RGB);
    // cv::flip(image, image, -1);
    // if (!image.empty())
    // {
    //   glBindTexture(GL_TEXTURE_2D, textureID);
    //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    //   glGenerateMipmap(GL_TEXTURE_2D);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // }
    // else
    // {
    //   std::cout << "Could not find embeded png texture '" << filename << "'" << std::endl;
    // }
  }

  // int width, height, nrComponents;
  // unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  // if (data)
  // {
  //   GLenum format;
  //   if (nrComponents == 1)
  //     format = GL_RED;
  //   else if (nrComponents == 3)
  //     format = GL_RGB;
  //   else if (nrComponents == 4)
  //     format = GL_RGBA;

  //   glBindTexture(GL_TEXTURE_2D, textureID);
  //   glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  //   glGenerateMipmap(GL_TEXTURE_2D);

  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //   stbi_image_free(data);
  // }
  // else
  // {
  //   std::cout << "Texture failed to load at path: '" << path << "'" << std::endl;
  //   stbi_image_free(data);

  //   const aiTexture *tex = scene->GetEmbeddedTexture(path);
  //   std::cout << scene->mNumTextures << std::endl;
  //   for(int i=0;i<scene->mNumTextures;i++){
  //     std::cout<<scene->mTextures[i]->mFilename.C_Str()<<std::endl;
  //   }
  //   if(tex->mHeight>0){
  //     GLenum format=GL_RGBA;
  //     glBindTexture(GL_TEXTURE_2D, textureID);
  //     // glTexImage2D(GL_TEXTURE_2D, 0, format, tex->mWidth, tex->mHeight, 0, format, GL_UNSIGNED_BYTE, tex->pcData);
  //     // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scene->mTextures[ti]->mWidth, scene->mTextures[ti]->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
  //     //              scene->mTextures[ti]->pcData);
  //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex->mWidth, tex->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
  //                  tex->pcData);

  //     glGenerateMipmap(GL_TEXTURE_2D);

  //     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (tex->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
  //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (tex->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP_TO_EDGE);

  //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //   }else{
  //   }

  return textureID;
}
#endif