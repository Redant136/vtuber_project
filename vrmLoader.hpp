#pragma once

#include <fstream>
#include <cstdint>

#include <GLFW/glfw3.h>

#include <nlohmann/json.hpp>

typedef nlohmann::json json;

namespace vtuber
{
  enum class Filetype
  {
    glb,
    vrm = glb,
    gltf
  };

  namespace gltf
  {
#define GLTF_COMPONENT_BYTE (5120)
#define GLTF_COMPONENT_UBYTE (5121)
#define GLTF_COMPONENT_SHORT (5122)
#define GLTF_COMPONENT_USHORT (5123)
#define GLTF_COMPONENT_INT (5124)
#define GLTF_COMPONENT_UINT (5125)
#define GLTF_COMPONENT_FLOAT (5126)
#define GLTF_COMPONENT_DOUBLE (5130)
    typedef std::string string;

    struct Extension
    {
      json data;
    };

    struct Buffer
    {
      uchar *buffer;
      //----------------
      string uri = "";
      int byteLength = 0;
      string name = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct BufferView
    {
      int buffer = 0;
      int byteOffset = 0;
      int byteLength = 0;
      int byteStride = 0;
      int target = 34962;
      string name = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct Accessor
    {
      int bufferView = -1;
      int byteOffset = 0;
      enum Types
      {
        SCALAR,
        VEC2,
        VEC3,
        VEC4,
        MAT2,
        MAT3,
        MAT4
      } type;
      enum glComponentType
      {
        BYTE = GLTF_COMPONENT_BYTE,
        UNSIGNED_BYTE = GLTF_COMPONENT_UBYTE,
        SHORT = GLTF_COMPONENT_SHORT,
        UNSIGNED_SHORT = GLTF_COMPONENT_USHORT,
        INT = GLTF_COMPONENT_INT,
        UNSIGNED_INT = GLTF_COMPONENT_UINT,
        FLOAT = GLTF_COMPONENT_FLOAT,
        DOUBLE = GLTF_COMPONENT_DOUBLE
      } componentType = FLOAT;
      int count = 0;
      std::vector<float> max;
      std::vector<float> min;
      bool normalized = false;
      struct Sparse
      {
        uint count = 0;
        struct Indices
        {
          int bufferView = -1;
          int byteOffset = 0;
          enum glComponentType
          {
            BYTE = GLTF_COMPONENT_BYTE,
            UNSIGNED_BYTE = GLTF_COMPONENT_UBYTE,
            SHORT = GLTF_COMPONENT_SHORT,
            UNSIGNED_SHORT = GLTF_COMPONENT_USHORT,
            INT = GLTF_COMPONENT_INT,
            UNSIGNED_INT = GLTF_COMPONENT_UINT,
            FLOAT = GLTF_COMPONENT_FLOAT,
            DOUBLE = GLTF_COMPONENT_DOUBLE
          } componentType;
          std::vector<Extension> extension;
          std::vector<Extension> extras;
        } indices;
        struct Values
        {
          int bufferView = -1;
          int byteOffset = 0;
          std::vector<Extension> extension;
          std::vector<Extension> extras;
        } values;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      } sparse;
      string name = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    //-----------------
    struct Image
    {
      string name = "";
      string uri = "";
      int bufferView = -1;
      string mimeType = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct Sampler
    {
      enum glFilter
      {
        NONE = 0,
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
      };
      enum glWrap
      {
        CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
        CLAMP_BORDER = GL_CLAMP_TO_BORDER,
        MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
        REPEAT = GL_REPEAT
      };
      glFilter magFilter = NEAREST;
      glFilter minFilter = NEAREST;
      glWrap wrapS = CLAMP_TO_EDGE;
      glWrap wrapT = CLAMP_TO_EDGE;
      string name = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct Texture
    {
      int sampler = -1;
      int source = -1;
      string name = "";
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct Material
    {
      struct TextureInfo
      {
        int index = -1;
        int texCoord = 0;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      struct PbrMetallicRoughness
      {
        TextureInfo baseColorTexture;
        std::vector<float> baseColorFactor = {1.f, 1.f, 1.f, 1.f};
        TextureInfo metallicRoughnessTexture;
        float metallicFactor = 1.f;
        float roughnessFactor = 1.f;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      struct MaterialNormalTextureInfo : public TextureInfo
      {
        float scale = 1.f;
      };
      struct MaterialOcclusionTextureInfo : public TextureInfo
      {
        float strength = 1.f;
      };
      string name = "";
      PbrMetallicRoughness pbrMetallicRoughness;
      MaterialNormalTextureInfo normalTexture;
      MaterialOcclusionTextureInfo occlusionTexture;
      TextureInfo emissiveTexture;
      std::vector<float> emissiveFactor;
      enum AlphaMode
      {
        OPAQUE,
        MASK,
        BLEND
      } alphaMode = OPAQUE;
      float alphaCutoff = 0.5f;
      bool doubleSided = false;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    //-------
    struct Camera
    {
      struct Orthographic
      {
        float xmag = 0.f;
        float ymag = 0.f;
        float zfar = 0.f;
        float znear = 0.f;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      struct Perspective
      {
        float aspectRatio = 1.f;
        float yfov = 1.f;
        float zfar = 0.f;
        float znear = 0.f;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      string name = "";
      Orthographic orthographic;
      Perspective perspective;
      enum ProjectionType
      {
        Perspective,
        Orthographic
      } type;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    //--------
    struct Skin
    {
      string name = "";
      int inverseBindMatrices = -1;
      std::vector<int> joints;
      int skeleton = -1;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    //--------
    struct Animation
    {
      struct AnimationChannel
      {
        int sampler = -1;
        struct AnimationTarget
        {
          int node = -1;
          enum
          {
            translation,
            rotation,
            scale,
            weights
          } path;
          std::vector<Extension> extension;
          std::vector<Extension> extras;
        } target;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      struct AnimationSampler
      {
        int input = -1;
        enum
        {
          LINEAR,
          STEP,
          CUBICSPLINE
        } interpolation;
        int output = -1;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      string name = "";
      std::vector<AnimationChannel> channels;
      std::vector<AnimationSampler> samplers;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    //--------
    struct Mesh
    {
      struct Primitive
      {
        struct MorphTarget
        {
          int POSITION = -1;
          int NORMAL = -1;
          int TANGENT = -1;
        };
        int mode = 4;
        int indices = -1;
        struct Attributes
        {
          int POSITION = -1;
          int NORMAL = -1;
          int TANGENT = -1;
          int TEXCOORD_0 = -1;
          int TEXCOORD_1 = -1;
          int COLOR_0 = -1;
          int JOINTS_0 = -1;
          int WEIGHTS_0 = -1;
        } attributes;
        int material = -1;
        std::vector<MorphTarget> targets;
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      };
      std::string name = "";
      std::vector<Primitive> primitives;
      std::vector<float> weights;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };
    struct Node
    {
      string name = "";
      std::vector<uint> children;
      std::vector<float> matrix = std::vector<float>({1, 0, 0, 0,
                                                      0, 1, 0, 0,
                                                      0, 0, 1, 0,
                                                      0, 0, 0, 1});
      std::vector<float> translation = std::vector<float>({0, 0, 0});
      std::vector<float> rotation = std::vector<float>({0, 0, 0, 1});
      std::vector<float> scale = std::vector<float>({1, 1, 1});
      int mesh = -1;
      int skin = -1;
      std::vector<float> weights;
      int camera = -1;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };

    struct Scene
    {
      string name = "";
      std::vector<int> nodes;
      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };

    struct glTFModel
    {
      struct
      {
        string version;
        string generator;
        string copyright;
        string minVersion;
        //---------------------------------
        std::vector<Extension> extension;
        std::vector<Extension> extras;
      } asset;
      std::vector<Buffer> buffers;
      std::vector<BufferView> bufferViews;

      std::vector<Accessor> accessors;
      std::vector<Texture> textures;
      std::vector<Sampler> samplers;
      std::vector<Image> images;
      std::vector<Material> materials;
      std::vector<Mesh> meshes;

      std::vector<Node> nodes;
      std::vector<Skin> skins;

      int scene = -1;
      std::vector<Scene> scenes;

      std::vector<Animation> animations;
      std::vector<Camera> cameras;
      std::vector<string> extensionsUsed;
      std::vector<string> extensionsRequired;

      std::vector<Extension> extension;
      std::vector<Extension> extras;
    };

    int getMeshPrimitiveAttribVal(const Mesh::Primitive::Attributes& attribute, std::string name, uint index)
    {
      int *attrib = (int*)&attribute;
#define _concatNames(a,b) a##b
#define parseAttribAlt(att_name) if(name==#att_name){attrib=(int*)(&attribute._concatNames(att_name,_0));}
#define parseAttrib(att_name) if(name==#att_name){attrib=(int*)(&attribute.att_name);}
      parseAttrib(POSITION)
      else parseAttrib(NORMAL)
      else parseAttrib(TANGENT)
      else parseAttribAlt(TEXCOORD)
      else parseAttribAlt(COLOR)
      else parseAttribAlt(JOINTS)
      else parseAttribAlt(WEIGHTS)
      else
      {
        std::cout<<"no such attribute"<<std::endl;
        assert(0);
      }
#undef parseAttrib
#undef parseAttribAlt
#undef _concatNames
      attrib += index;
      return *attrib;
    }

    uint gltf_sizeof(int type)
    {
      switch (type)
      {
      case GLTF_COMPONENT_BYTE:
      case GLTF_COMPONENT_UBYTE:
        return sizeof(char);
      case GLTF_COMPONENT_SHORT:
      case GLTF_COMPONENT_USHORT:
        return sizeof(short);
      case GLTF_COMPONENT_INT:
      case GLTF_COMPONENT_UINT:
        return sizeof(int);
      case GLTF_COMPONENT_FLOAT:
        return sizeof(float);
      case GLTF_COMPONENT_DOUBLE:
        return sizeof(double);
      default:
        assert(0);
        return 0;
      }
    }

    uint gltf_num_components(Accessor::Types type)
    {
      switch(type){
        case Accessor::Types::SCALAR:
        return 1;
        case Accessor::Types::VEC2:
          return 2;
        case Accessor::Types::VEC3:
          return 3;
        case Accessor::Types::VEC4:
          return 4;
        case Accessor::Types::MAT2:
          return 4;
        case Accessor::Types::MAT3:
          return 9;
        case Accessor::Types::MAT4:
          return 16;
        default:
          return 0;
      }
    }

    uchar *getDataFromAccessor(const glTFModel &model, const Accessor &accessor, uint index = 0)
    {
      const BufferView &bfView = model.bufferViews[accessor.bufferView];
      uint byteStride = bfView.byteStride;
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

      return (model.buffers[bfView.buffer].buffer + bfView.byteOffset) + accessor.byteOffset + (byteStride)*index;
    }
  }

  class Importer
  {
  public:
    gltf::glTFModel model;

    void import(std::string path, Filetype type = Filetype::glb)
    {
      this->path = path;
      std::ifstream input(path, std::ios::binary);
      std::vector<char> bytes(
          (std::istreambuf_iterator<char>(input)),
          (std::istreambuf_iterator<char>()));

      input.close();

      switch (type)
      {
      case Filetype::glb:
        parseGLB(bytes);
        break;
      case Filetype::gltf:
        parseGLTF(bytes);
        break;
      default:
        break;
      }
    }

  private:
    std::string path = "";

    template <typename T>
    std::vector<T> pop(std::vector<T> &buffer, unsigned int size = 1)
    {
      std::vector<T> r = std::vector<T>(buffer.begin(), buffer.begin() + size);
      buffer.erase(buffer.begin(), buffer.begin() + size);
      return r;
    }

    std::vector<gltf::Extension> deserializeExtensions(json extensions)
    {
      // TODO(ANT) fix this and research
      std::vector<gltf::Extension> vec;
      for (uint i = 0; i < extensions.size(); i++)
      {
        // glTF::Extension ext;
        // ext.data=extensions[i];
        // vec.push_back(ext);
      }
      return vec;
    }

    gltf::glTFModel parseGLTFJSON(json glTF_data)
    {
#define jsonCondition(item, id) if (std::string(item.key()) == #id)
#define jsonFunctionMacro(item, id, func) \
  jsonCondition(item, id)                 \
  {                                       \
    func;                                 \
    continue;                             \
  }
#define jsonExtensionMacro(item, dst, id) jsonFunctionMacro(item, id, dst.id = deserializeExtensions(item.value());)
#define jsonConvertMacro(item, dst, id, type) jsonFunctionMacro(item, id, dst.id = item.value().get<type>();)
#define jsonConvertCastMacro(item, dst, id, type, castType) jsonFunctionMacro(item, id, dst.id = (castType)item.value().get<type>();)
#define jsonArrayMacro(item, dst, id, type, i) jsonFunctionMacro( \
    item, id, dst.id.resize(item.value().size()); for (uint i = 0; i < item.value().size(); i++) { dst.id[i] = item.value()[i].get<type>(); });

      gltf::glTFModel gltf;

      // parse json in object
      for (auto &item : glTF_data.items())
      {
        jsonCondition(item, asset)
        {
          for (auto &x : glTF_data["asset"].items())
          {
            jsonConvertMacro(x, gltf.asset, version, std::string);
            jsonConvertMacro(x, gltf.asset, generator, std::string);
            jsonConvertMacro(x, gltf.asset, copyright, std::string);
            jsonConvertMacro(x, gltf.asset, minVersion, std::string);
            jsonExtensionMacro(x, gltf.asset, extension);
            jsonExtensionMacro(x, gltf.asset, extras);
            
          }
          continue;
        }
        jsonCondition(item, buffers)
        {
          for (uint i = 0; i < glTF_data["buffers"].size(); i++)
          {
            gltf::Buffer buffer;
            for (auto &x : glTF_data["buffers"][i].items())
            {
              jsonConvertMacro(x, buffer, uri, std::string);
              jsonConvertMacro(x, buffer, byteLength, int);
              jsonConvertMacro(x, buffer, name, std::string);
              jsonExtensionMacro(x, buffer, extension);
              jsonExtensionMacro(x, buffer, extras);
            }
            gltf.buffers.push_back(buffer);
          }
          continue;
        }
        jsonCondition(item, bufferViews)
        {
          for (uint i = 0; i < glTF_data["bufferViews"].size(); i++)
          {
            gltf::BufferView bufferV;
            for (auto &x : glTF_data["bufferViews"][i].items())
            {
              jsonConvertMacro(x, bufferV, buffer, int);
              jsonConvertMacro(x, bufferV, byteOffset, int);
              jsonConvertMacro(x, bufferV, byteLength, int);
              jsonConvertMacro(x, bufferV, byteStride, int);
              jsonConvertMacro(x, bufferV, target, int);
              jsonConvertMacro(x, bufferV, name, std::string);
              jsonExtensionMacro(x, bufferV, extension);
              jsonExtensionMacro(x, bufferV, extras);
            }
            gltf.bufferViews.push_back(bufferV);
          }
          continue;
        }
        jsonCondition(item, accessors)
        {
          for (uint i = 0; i < glTF_data["accessors"].size(); i++)
          {
            gltf::Accessor accessor;
            for (auto &x : glTF_data["accessors"][i].items())
            {
              jsonConvertMacro(x, accessor, bufferView, int);
              jsonConvertMacro(x, accessor, byteOffset, int);
              if (std::string(x.key()) == "type")
              {
                if (x.value().get<std::string>() == "SCALAR")
                {
                  accessor.type = gltf::Accessor::Types::SCALAR;
                }
                else if (x.value().get<std::string>() == "VEC2")
                {
                  accessor.type = gltf::Accessor::Types::VEC2;
                }
                else if (x.value().get<std::string>() == "VEC3")
                {
                  accessor.type = gltf::Accessor::Types::VEC3;
                }
                else if (x.value().get<std::string>() == "VEC4")
                {
                  accessor.type = gltf::Accessor::Types::VEC4;
                }
                else if (x.value().get<std::string>() == "MAT2")
                {
                  accessor.type = gltf::Accessor::Types::MAT2;
                }
                else if (x.value().get<std::string>() == "MAT3")
                {
                  accessor.type = gltf::Accessor::Types::MAT3;
                }
                else if (x.value().get<std::string>() == "MAT4")
                {
                  accessor.type = gltf::Accessor::Types::MAT4;
                }
                continue;
              }
              // jsonConvertMacro(x, accessor, type, std::string);
              jsonConvertCastMacro(x, accessor, componentType, int, gltf::Accessor::glComponentType);
              jsonConvertMacro(x, accessor, count, int);
              jsonArrayMacro(x, accessor, max, float, j);
              jsonArrayMacro(x, accessor, min, float, j);
              // if (std::string(x.key()) == "max")
              // {
              //   std::vector<float> max;
              //   for (uint j = 0; j < x.value().size(); j++)
              //   {
              //     max.push_back(x.value()[j].get<float>());
              //   }
              //   accessor.max = max;
              //   continue;
              // }
              // if (std::string(x.key()) == "min")
              // {
              //   std::vector<float> min;
              //   for (uint j = 0; j < x.value().size(); j++)
              //   {
              //     min.push_back(x.value()[j].get<float>());
              //   }
              //   accessor.min = min;
              //   continue;
              // }
              jsonConvertMacro(x, accessor, normalized, bool);
              if (std::string(x.key()) == "sparse")
              {
                jsonConvertMacro(x, accessor.sparse, count, int);
                if (std::string(x.key()) == "indices")
                {
                  for (auto &y : x.value().items())
                  {
                    jsonConvertMacro(y, accessor.sparse.indices, bufferView, int);
                    jsonConvertMacro(y, accessor.sparse.indices, byteOffset, int);
                    jsonConvertCastMacro(y, accessor.sparse.indices, componentType, int, gltf::Accessor::Sparse::Indices::glComponentType)
                        jsonExtensionMacro(y, accessor.sparse.indices, extension);
                    jsonExtensionMacro(y, accessor.sparse.indices, extras);
                  }
                }
                if (std::string(x.key()) == "values")
                {
                  for (auto &y : x.value().items())
                  {
                    jsonConvertMacro(y, accessor.sparse.values, bufferView, int);
                    jsonConvertMacro(y, accessor.sparse.values, byteOffset, int);
                    jsonExtensionMacro(y, accessor.sparse.values, extension);
                    jsonExtensionMacro(y, accessor.sparse.values, extras);
                  }
                }
                jsonExtensionMacro(x, accessor.sparse, extension);
                jsonExtensionMacro(x, accessor.sparse, extras);
                continue;
              }
              jsonConvertMacro(x, accessor, name, std::string);
              jsonExtensionMacro(x, accessor, extension);
              jsonExtensionMacro(x, accessor, extras);
            }
            gltf.accessors.push_back(accessor);
          }
          continue;
        }
        jsonCondition(item, textures)
        {
          for (uint i = 0; i < glTF_data["textures"].size(); i++)
          {
            gltf::Texture texture;
            for (auto &x : glTF_data["textures"][i].items())
            {
              jsonConvertMacro(x, texture, sampler, int);
              jsonConvertMacro(x, texture, source, int);
              jsonConvertMacro(x, texture, name, std::string);
              jsonExtensionMacro(x, texture, extension);
              jsonExtensionMacro(x, texture, extras);
            }
            gltf.textures.push_back(texture);
          }
          continue;
        }
        jsonCondition(item, samplers)
        {
          for (uint i = 0; i < glTF_data["samplers"].size(); i++)
          {
            gltf::Sampler sampler;
            for (auto &x : glTF_data["samplers"][i].items())
            {
              jsonConvertCastMacro(x, sampler, magFilter, int, gltf::Sampler::glFilter);
              jsonConvertCastMacro(x, sampler, minFilter, int, gltf::Sampler::glFilter);
              jsonConvertCastMacro(x, sampler, wrapS, int, gltf::Sampler::glWrap);
              jsonConvertCastMacro(x, sampler, wrapT, int, gltf::Sampler::glWrap);
              jsonConvertMacro(x, sampler, name, std::string);
              jsonExtensionMacro(x, sampler, extension);
              jsonExtensionMacro(x, sampler, extras);
            }
            gltf.samplers.push_back(sampler);
          }
          continue;
        }
        jsonCondition(item, images)
        {
          for (uint i = 0; i < glTF_data["images"].size(); i++)
          {
            gltf::Image image;
            for (auto &x : glTF_data["images"][i].items())
            {
              jsonConvertMacro(x, image, name, std::string);
              jsonConvertMacro(x, image, uri, std::string);
              jsonConvertMacro(x, image, bufferView, int);
              jsonConvertMacro(x, image, mimeType, std::string);
              jsonExtensionMacro(x, image, extension);
              jsonExtensionMacro(x, image, extras);
            }
            gltf.images.push_back(image);
          }
          continue;
        }
        jsonCondition(item, materials)
        {
          for (uint i = 0; i < glTF_data["materials"].size(); i++)
          {
            gltf::Material material;
            for (auto &x : glTF_data["materials"][i].items())
            {
              jsonConvertMacro(x, material, name, std::string);
              if (std::string(x.key()) == "pbrMetallicRoughness")
              {
                for (auto &y : x.value().items())
                {
                  if (std::string(y.key()) == "baseColorTexture")
                  {
                    for (auto &z : y.value().items())
                    {
                      jsonConvertMacro(z, material.pbrMetallicRoughness.baseColorTexture, index, int);
                      jsonConvertMacro(z, material.pbrMetallicRoughness.baseColorTexture, texCoord, int);
                      jsonExtensionMacro(z, material.pbrMetallicRoughness.baseColorTexture, extension);
                      jsonExtensionMacro(z, material.pbrMetallicRoughness.baseColorTexture, extras);
                    }
                    continue;
                  }
                  if (std::string(y.key()) == "baseColorFactor")
                  {
                    std::vector<float> baseColorFactor;
                    for (uint j = 0; j < y.value().size(); j++)
                    {
                      baseColorFactor.push_back(y.value()[j].get<float>());
                    }
                    material.pbrMetallicRoughness.baseColorFactor = baseColorFactor;
                    continue;
                  }
                  if (std::string(y.key()) == "metallicRoughnessTexture")
                  {
                    for (auto &z : y.value().items())
                    {
                      jsonConvertMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, index, int);
                      jsonConvertMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, texCoord, int);
                      jsonExtensionMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, extension);
                      jsonExtensionMacro(z, material.pbrMetallicRoughness.metallicRoughnessTexture, extras);
                    }
                    continue;
                  }
                  jsonConvertMacro(y, material.pbrMetallicRoughness, metallicFactor, float);
                  jsonConvertMacro(y, material.pbrMetallicRoughness, roughnessFactor, float);
                  jsonExtensionMacro(y, material.pbrMetallicRoughness, extension);
                  jsonExtensionMacro(y, material.pbrMetallicRoughness, extras);
                }
                continue;
              }
              if (std::string(x.key()) == "normalTexture")
              {
                for (auto &y : x.value().items())
                {
                  jsonConvertMacro(y, material.normalTexture, scale, float);
                  jsonConvertMacro(y, material.normalTexture, index, int);
                  jsonConvertMacro(y, material.normalTexture, texCoord, int);
                  jsonExtensionMacro(y, material.normalTexture, extension);
                  jsonExtensionMacro(y, material.normalTexture, extras);
                }
                continue;
              }
              if (std::string(x.key()) == "occlusionTexture")
              {
                for (auto &y : x.value().items())
                {
                  jsonConvertMacro(y, material.occlusionTexture, strength, float);
                  jsonConvertMacro(y, material.occlusionTexture, index, int);
                  jsonConvertMacro(y, material.occlusionTexture, texCoord, int);
                  jsonExtensionMacro(y, material.occlusionTexture, extension);
                  jsonExtensionMacro(y, material.occlusionTexture, extras);
                }
                continue;
              }
              if (std::string(x.key()) == "emissiveTexture")
              {
                for (auto &y : x.value().items())
                {
                  jsonConvertMacro(y, material.emissiveTexture, index, int);
                  jsonConvertMacro(y, material.emissiveTexture, texCoord, int);
                  jsonExtensionMacro(y, material.emissiveTexture, extension);
                  jsonExtensionMacro(y, material.emissiveTexture, extras);
                }
                continue;
              }
              jsonArrayMacro(x, material, emissiveFactor, float, j);
              // if (std::string(x.key()) == "emissiveFactor")
              // {
              //   std::vector<float> emissiveFactor;
              //   for (uint j = 0; j < x.value().size(); j++)
              //   {
              //     emissiveFactor.push_back(x.value()[j]);
              //   }
              //   material.emissiveFactor = emissiveFactor;
              //   continue;
              // }
              if (std::string(x.key()) == "alphaMode")
              {
                if (x.value().get<std::string>() == "OPAQUE")
                {
                  material.alphaMode = gltf::Material::AlphaMode::OPAQUE;
                }
                else if (x.value().get<std::string>() == "MASK")
                {
                  material.alphaMode = gltf::Material::AlphaMode::MASK;
                }
                else if (x.value().get<std::string>() == "BLEND")
                {
                  material.alphaMode = gltf::Material::AlphaMode::BLEND;
                }
                continue;
              }
              jsonConvertMacro(x, material, alphaCutoff, float);
              jsonConvertMacro(x, material, doubleSided, bool);
              jsonExtensionMacro(x, material, extension);
              jsonExtensionMacro(x, material, extras);
            }
            gltf.materials.push_back(material);
          }
          continue;
        }
        jsonCondition(item, meshes)
        {
          for (uint i = 0; i < glTF_data["meshes"].size(); i++)
          {
            gltf::Mesh mesh;
            for (auto &x : glTF_data["meshes"][i].items())
            {
              jsonConvertMacro(x, mesh, name, std::string);
              if (std::string(x.key()) == "primitives")
              {
                for (uint j = 0; j < x.value().size(); j++)
                {
                  gltf::Mesh::Primitive primitive;
                  for (auto &y : x.value()[j].items())
                  {
                    jsonConvertMacro(y, primitive, mode, int);
                    jsonConvertMacro(y, primitive, indices, int);
                    if (std::string(y.key()) == "attributes")
                    {
                      for (auto &z : y.value().items())
                      {
                        jsonConvertMacro(z, primitive.attributes, POSITION, int);
                        jsonConvertMacro(z, primitive.attributes, NORMAL, int);
                        jsonConvertMacro(z, primitive.attributes, TANGENT, int);
                        jsonConvertMacro(z, primitive.attributes, TEXCOORD_0, int);
                        jsonConvertMacro(z, primitive.attributes, TEXCOORD_1, int);
                        jsonConvertMacro(z, primitive.attributes, COLOR_0, int);
                        jsonConvertMacro(z, primitive.attributes, JOINTS_0, int);
                        jsonConvertMacro(z, primitive.attributes, WEIGHTS_0, int);
                      }
                    }
                    jsonConvertMacro(y, primitive, material, int);
                    if (std::string(y.key()) == "targets")
                    {
                      for (uint k = 0; k < y.value().size(); k++)
                      {
                        gltf::Mesh::Primitive::MorphTarget target;
                        for (auto &z : y.value()[k].items())
                        {
                          jsonConvertMacro(z, target, POSITION, int);
                          jsonConvertMacro(z, target, NORMAL, int);
                          jsonConvertMacro(z, target, TANGENT, int);
                        }
                        primitive.targets.push_back(target);
                      }
                    }
                    jsonExtensionMacro(y, primitive, extension);
                    jsonExtensionMacro(y, primitive, extras);
                  }
                  mesh.primitives.push_back(primitive);
                }
                continue;
              }
              jsonArrayMacro(x, mesh, weights, float, j);
              // if (std::string(x.key()) == "weights")
              // {
              //   std::vector<float> weights;
              //   for (uint j = 0; j < x.value().size(); j++)
              //   {
              //     weights.push_back(x.value()[j].get<float>());
              //   }
              //   mesh.weights = weights;
              // }
              jsonExtensionMacro(x, mesh, extension);
              jsonExtensionMacro(x, mesh, extras);
            }
            gltf.meshes.push_back(mesh);
          }
          continue;
        }
        jsonCondition(item, nodes)
        {
          for (uint i = 0; i < glTF_data["nodes"].size(); i++)
          {
            gltf::Node node;
            for (auto &x : glTF_data["nodes"][i].items())
            {
              jsonConvertMacro(x, node, name, std::string);
              jsonArrayMacro(x, node, children, int, j);
              jsonArrayMacro(x, node, matrix, float, j);
              jsonArrayMacro(x, node, translation, float, j);
              jsonArrayMacro(x, node, rotation, float, j);
              jsonArrayMacro(x, node, scale, float, j);
              jsonConvertMacro(x, node, mesh, int);
              jsonConvertMacro(x, node, skin, int);
              jsonArrayMacro(x, node, weights, float, j);
              jsonConvertMacro(x, node, camera, int);
              jsonExtensionMacro(x, node, extension);
              jsonExtensionMacro(x, node, extras);
            }
            if (node.matrix.size() == 0)
            {
              // node.matrix = std::vector<float>(
              //     {1, 0, 0, 0,
              //      0, 1, 0, 0,
              //      0, 0, 1, 0,
              //      0, 0, 0, 1});
            }
            gltf.nodes.push_back(node);
          }
          continue;
        }
        jsonCondition(item, skins)
        {
          for (uint i = 0; i < glTF_data["skins"].size(); i++)
          {
            gltf::Skin skin;
            for (auto &x : glTF_data["skins"][i].items())
            {
              jsonConvertMacro(x, skin, name, std::string);
              jsonConvertMacro(x, skin, inverseBindMatrices, int);
              jsonArrayMacro(x, skin, joints, int, j);
              jsonConvertMacro(x, skin, skeleton, int);
              jsonExtensionMacro(x, skin, extension);
              jsonExtensionMacro(x, skin, extras);
            }
            gltf.skins.push_back(skin);
          }
          continue;
        }
        jsonCondition(item, scene)
        {
          gltf.scene = glTF_data["scene"].get<int>();
          continue;
        }
        jsonCondition(item, scenes)
        {
          for (uint i = 0; i < glTF_data["scenes"].size(); i++)
          {
            gltf::Scene scene;
            for (auto &x : glTF_data["scenes"][i].items())
            {
              jsonConvertMacro(x, scene, name, std::string);
              jsonArrayMacro(x, scene, nodes, int, j);
              jsonExtensionMacro(x, scene, extension);
              jsonExtensionMacro(x, scene, extras);
            }
            gltf.scenes.push_back(scene);
          }
          continue;
        }
        jsonCondition(item, animations)
        {
          for (uint i = 0; i < glTF_data["animations"].size(); i++)
          {
            gltf::Animation animation;
            for (auto &x : glTF_data["animations"][i].items())
            {
              jsonConvertMacro(x, animation, name, std::string);
              if (std::string(x.key()) == "channels")
              {
                for (uint j = 0; j < x.value().size(); j++)
                {
                  gltf::Animation::AnimationChannel channel;
                  for (auto &y : x.value()[j].items())
                  {
                    jsonConvertMacro(y, channel, sampler, int);
                    if (std::string(y.key()) == "target")
                    {
                      for (auto &z : y.value().items())
                      {
                        jsonConvertMacro(z, channel.target, node, int);
                        if (std::string(z.key()) == "path")
                        {
                          if (z.value().get<std::string>() == "translation")
                          {
                            channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::translation;
                          }
                          else if (z.value().get<std::string>() == "rotation")
                          {
                            channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::rotation;
                          }
                          else if (z.value().get<std::string>() == "scale")
                          {
                            channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::scale;
                          }
                          else if (z.value().get<std::string>() == "weights")
                          {
                            channel.target.path = gltf::Animation::AnimationChannel::AnimationTarget::weights;
                          }
                          continue;
                        }
                        jsonExtensionMacro(z, channel.target, extension);
                        jsonExtensionMacro(z, channel.target, extras);
                      }
                      continue;
                    }
                    jsonExtensionMacro(y, channel, extension);
                    jsonExtensionMacro(y, channel, extras);
                  }
                  animation.channels.push_back(channel);
                }
                continue;
              }
              if (std::string(x.key()) == "samplers")
              {
                for (uint j = 0; j < x.value().size(); j++)
                {
                  gltf::Animation::AnimationSampler sampler;
                  for (auto &y : x.value()[j].items())
                  {
                    jsonConvertMacro(y, sampler, input, int);
                    if (std::string(y.key()) == "interpolation")
                    {
                      if (y.value().get<std::string>() == "LINEAR")
                      {
                        sampler.interpolation = gltf::Animation::AnimationSampler::LINEAR;
                      }
                      else if (y.value().get<std::string>() == "STEP")
                      {
                        sampler.interpolation = gltf::Animation::AnimationSampler::STEP;
                      }
                      else if (y.value().get<std::string>() == "CUBICSPLINE")
                      {
                        sampler.interpolation = gltf::Animation::AnimationSampler::CUBICSPLINE;
                      }
                      continue;
                    }
                    jsonConvertMacro(y, sampler, output, int);
                    jsonExtensionMacro(y, sampler, extension);
                    jsonExtensionMacro(y, sampler, extras);
                  }
                  animation.samplers.push_back(sampler);
                }
                continue;
              }
              jsonExtensionMacro(x, animation, extension);
              jsonExtensionMacro(x, animation, extras);
            }
            gltf.animations.push_back(animation);
          }
          continue;
        }
        jsonCondition(item, cameras)
        {
          for (uint i = 0; i < glTF_data["cameras"].size(); i++)
          {
            gltf::Camera camera;
            for (auto &x : glTF_data["cameras"][i].items())
            {
              jsonConvertMacro(x, camera, name, std::string);
              if (std::string(x.key()) == "orthographic")
              {
                for (auto &y : x.value().items())
                {
                  jsonConvertMacro(y, camera.orthographic, xmag, float);
                  jsonConvertMacro(y, camera.orthographic, ymag, float);
                  jsonConvertMacro(y, camera.orthographic, zfar, float);
                  jsonConvertMacro(y, camera.orthographic, znear, float);
                  jsonExtensionMacro(y, camera.orthographic, extension);
                  jsonExtensionMacro(y, camera.orthographic, extras);
                }
                continue;
              }
              if (std::string(x.key()) == "perspective")
              {
                for (auto &y : x.value().items())
                {
                  jsonConvertMacro(y, camera.perspective, aspectRatio, float);
                  jsonConvertMacro(y, camera.perspective, yfov, float);
                  jsonConvertMacro(y, camera.perspective, zfar, float);
                  jsonConvertMacro(y, camera.perspective, znear, float);
                  jsonExtensionMacro(y, camera.perspective, extension);
                  jsonExtensionMacro(y, camera.perspective, extras);
                }
                continue;
              }
              if (std::string(x.key()) == "type")
              {
                if (x.value().get<std::string>() == "Perspective")
                {
                  camera.type = gltf::Camera::Perspective;
                }
                else if (x.value().get<std::string>() == "Orthographic")
                {
                  camera.type = gltf::Camera::Orthographic;
                }
                continue;
              }
              jsonExtensionMacro(x, camera, extension);
              jsonExtensionMacro(x, camera, extras);
            }
            gltf.cameras.push_back(camera);
          }
          continue;
        }
        jsonArrayMacro(item, gltf, extensionsUsed, std::string, i);
        jsonArrayMacro(item, gltf, extensionsRequired, std::string, i);
        jsonExtensionMacro(item, gltf, extension);
        jsonExtensionMacro(item, gltf, extras);
      }

#undef jsonNodeArrayMacro
#undef jsonFunctionMacro
#undef jsonExtensionMacro
#undef jsonConvertMacro
#undef jsonConvertCastMacro
#undef jsonCondition

      // fix image name of older versions
      // for (uint i = 0; i < gltf.images.size(); i++)
      // {
      //   if (gltf.images[i].name.empty())
      //   {
      //     std::string extraName = glTF_data["images"][i]["extra"]["name"].get<std::string>();
      //     if (!extraName.empty())
      //     {
      //       gltf.images[i].name = extraName;
      //     }
      //   }
      // }

      // fix unique mesh names
      // too lazy to implement
      std::string *names = new std::string[gltf.meshes.size()];
      for (uint i = 0; i < gltf.meshes.size(); i++)
      {
        for (uint j = 0; j < i; j++)
        {
          if (gltf.meshes[i].name == names[j])
          {
            throw std::invalid_argument("duplicate name");
          }
        }
        names[i] = gltf.meshes[i].name;
      }
      delete[] names;

      return gltf;
    }

    void parseGLTF(std::vector<char> buffer)
    {
      auto json = json::parse(buffer.begin(), buffer.end());
      this->model = parseGLTFJSON(json);

      std::string dir = "./";
      {
        char sep = '/';
#ifdef _WIN32
        sep = '\\';
#endif
        size_t sepIndex = path.rfind(sep, path.length());
        if (sepIndex != std::string::npos)
        {
          dir = path.substr(0, sepIndex);
        }
      }
      
      for (uint i = 0; i < model.buffers.size(); i++)
      {
        if (model.buffers[i].uri.find("data:") == 0) // data URI
        {
          if (model.buffers[i].uri.find("base64") == std::string::npos)
          {
            assert(0 && "can only decode base64");
          }
          std::string data = model.buffers[i].uri.substr(model.buffers[i].uri.find(",") + 1);
          data = base64::decode(data);
          model.buffers[i].buffer = new uchar[data.length()];
          memcpy(model.buffers[i].buffer, data.data(), data.length());
        }
        else // file based
        {
          std::string file = dir + model.buffers[i].uri;
          std::ifstream input(file, std::ios::binary);
          std::vector<char> bytes(
              (std::istreambuf_iterator<char>(input)),
              (std::istreambuf_iterator<char>()));
          input.close();

          model.buffers[i].buffer=new uchar[bytes.size()];
          memcpy(model.buffers[i].buffer,bytes.data(),bytes.size());
        }
      }
    }

    void parseGLB(std::vector<char> buffer)
    {
      typedef uint8_t u8;
      typedef uint32_t u32;

      struct
      {
        char glTF[4];
        u32 version;
        u32 dataSize;
      } header;
      memcpy(&header, pop(buffer, 12).data(), 12);

      if (std::string(header.glTF) == "glTF")
        throw std::invalid_argument("not gltf");
      if (header.version == 1)
        throw std::invalid_argument("will have problems because gltf v1");

      struct chunck
      {
        u32 chunckLength;
        u32 chuckType;
        u8 *data;
      };

      std::vector<chunck> chuncks = std::vector<chunck>();
      while (buffer.size() > 0)
      {
        chunck c;
        memcpy(&c, pop(buffer, 8).data(), 8);
        c.data = new u8[c.chunckLength];
        memcpy(c.data, pop(buffer, c.chunckLength).data(), c.chunckLength);

        chuncks.push_back(c);
      }

      if (chuncks.size() != 2)
        throw std::invalid_argument("unknown chunk count: " + chuncks.size());
      if (chuncks[0].chuckType != 0x4E4F534A)
        throw std::invalid_argument("chunk 0 is not JSON");
      if (chuncks[1].chuckType != 0x004E4942)
        throw std::invalid_argument("chunk 1 is not BIN");

      auto json = json::parse(chuncks[0].data, chuncks[0].data + chuncks[0].chunckLength);
      this->model = parseGLTFJSON(json);

      if (model.buffers.size() != 1)
        throw std::invalid_argument("only 1 buffer should be present");

      model.buffers[0].buffer = new uchar[chuncks[1].chunckLength];
      memcpy(model.buffers[0].buffer, chuncks[1].data, chuncks[1].chunckLength);

      // free mem
      for (chunck c : chuncks)
      {
        delete[] c.data;
      }
    }

    //TODO gltf v1
  };
};
