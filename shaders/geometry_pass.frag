#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;

// color
uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;
uniform float alphaCutoff;

// textures
uniform sampler2D texture_base;
uniform sampler2D texture_normal;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

// KHR_texture_transform
uniform bool KHR_texture_transform;
uniform vec2 u_offset, u_scale;
uniform float u_rotation;

void main()
{    
  vec2 UV=TexCoords;
  if(KHR_texture_transform){
    UV = (
      mat3(1,0,0, 0,1,0, u_offset.x, u_offset.y, 1)*
      mat3( cos(u_rotation), sin(u_rotation), 0,
            -sin(u_rotation), cos(u_rotation), 0,
            0,             0, 1)*
      mat3(u_scale.x,0,0, 0,u_scale.y,0, 0,0,1)*
      vec3(TexCoords,1.0)).xy;
  }

  vec4 color = texture(texture_base, UV);
  if(!KHR_materials_unlit)
    color += texture(texture_emisive,UV);
  if(!hasBaseColorTexture){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }
  vec3 normal=Normal+texture(texture_normal,UV).xyz;

  if(color.w<alphaCutoff){
    discard;
  }


  gPosition = Pos;
  gNormal = normalize(normal);
  gAlbedoSpec = color;
}