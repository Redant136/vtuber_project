#version 330 core
#define FloatPrecision 0.1
out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 baseColorFactor;
uniform bool hasMaterial;

uniform sampler2D texture_base;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_normal;

vec4 blend(vec4 source, vec4 dest, float alpha)
{
    return source * alpha + dest * (1.0-alpha);
}

bool is0(float f){
  if(f<=FloatPrecision)
    return true;
  else
    return false;
}
bool is0(vec3 v){
  if(is0(v.x)&&is0(v.y)&&is0(v.z)){
    return true;
  }
  else{
    return false;
  }
}
bool is0(vec4 v){
  if(is0(v.x)&&is0(v.y)&&is0(v.z)&&is0(v.w)){
    return true;
  }
  else{
    return false;
  }
}

void main()
{
  vec4 color = texture(texture_base, TexCoords) + texture(texture_emisive,TexCoords);
  if(!hasMaterial){
    color=baseColorFactor;
  }else{
    color*=baseColorFactor;
  }
  FragColor=color;
}