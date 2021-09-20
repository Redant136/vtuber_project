#version 330 core
#define MAX_LIGHT_SOURCES 8

out vec4 FragColor;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;

// lighting
uniform vec4 lightSources[MAX_LIGHT_SOURCES];
uniform int lightSourcesLength;

// color
uniform vec4 baseColorFactor;
uniform bool hasBaseColorTexture;

// textures
uniform sampler2D texture_base;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_normal;

// extensions
// KHR_materials_unlit
uniform bool KHR_materials_unlit;

// KHR_texture_transform
uniform bool KHR_texture_transform;
uniform vec2 u_offset, u_scale;
uniform float u_rotation;

vec4 blend(vec4 source, vec4 dest, float alpha)
{
    return source * alpha + dest * (1.0-alpha);
}

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

  if(lightSourcesLength<lightSources.length() && !KHR_materials_unlit){
    for(int i=0;i<lightSourcesLength;i++){
      vec3 lightDirection=normalize(vec3(lightSources[i])-Pos);
      vec4 ambient=lightSources[i].w*vec4(1,1,1,1);
      vec4 diffuse=max(dot(normalize(Normal),lightDirection),0.f)*vec4(1,1,1,1);
      color=(ambient+diffuse)*color;
    }
  }


  FragColor = color;
}