#version 330 core
#define MAX_LIGHT_SOURCES 8

// out vec4 FragColor;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 Pos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 Color;
in vec3 BitTangent;

// color
struct Light {
    vec3 Position;
    vec3 Color;
    float Intensity;
};
uniform Light lights[MAX_LIGHT_SOURCES];
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

// VRM
uniform bool VRM;
uniform float shadeToony;
uniform float shadeShift;
uniform vec4 materialColor;
uniform vec4 shadeColor;

float lerp(float a,float b,float w){
    return a+w*(b-a);
}
vec3 lerp(vec3 a,vec3 b,float w){
    return a+w*(b-a);
}
vec4 lerp(vec4 a,vec4 b,float w){
    return a+w*(b-a);
}
float distanceSquare(vec2 a,vec2 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y);
}
float distanceSquare(vec3 a,vec3 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z);
}
float distanceSquare(vec4 a,vec4 b){
    return (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)+(a.z-b.z)*(a.z-b.z)+(a.w-b.w)*(a.w-b.w);
}
bool floatEquals(float a,float b){
    return (a-b)<0.001&&(a-b)>-0.001;
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
  vec3 normal=Normal+texture(texture_normal,UV).xyz;

  if(color.w<alphaCutoff){
    discard;
  }

  if(VRM){
    const float shadeRate=0.2;
    vec3 lighting  = vec3(0,0,0);
    for(int i = 0; i < lights.length(); i++)
    {
        if(lights[i].Intensity<=0){
            continue;
        }
        vec3 lightDir = normalize(lights[i].Position - Pos);
        float lightIntensity = max(dot(Normal, lightDir), 0.f) * lights[i].Intensity;
        lightIntensity = clamp((lightIntensity - shadeShift) / max(0.000001, (lerp(1,shadeShift,shadeToony) - shadeShift)),0,1);
        lightIntensity = (int(lightIntensity/shadeRate))*shadeRate;

        lighting+=lerp(shadeColor,materialColor,lightIntensity).xyz;
        color.xyz=lighting*color.xyz;
    }
  }
  else if(!KHR_materials_unlit){
    for(int i=0;i<lights.length();i++){
      if(lights[i].Intensity<=0){
        continue;
      }
      vec3 lightDirection=normalize(lights[i].Position - Pos);
      vec3 ambient=lights[i].Intensity*vec3(1,1,1);
      vec3 diffuse=max(dot(normalize(normal),lightDirection),0.f)*lights[i].Color.xyz;
      color.xyz=(ambient+diffuse)*color.xyz;
    }
  }

  // FragColor=color;
  gPosition = Pos;
  gNormal = normalize(normal);
  gAlbedoSpec = color;
}