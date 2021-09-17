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
uniform bool hasMaterial;

// textures
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

void main()
{
  vec4 color = texture(texture_base, TexCoords) + texture(texture_emisive,TexCoords);
  if(!hasMaterial){
    color = baseColorFactor;
  }else{
    color *= baseColorFactor;
  }

  if(lightSourcesLength<lightSources.length()){
    for(int i=0;i<lightSourcesLength;i++){
      vec3 lightDirection=normalize(vec3(lightSources[i])-Pos);
      vec4 ambient=lightSources[i].w*vec4(1,1,1,1);
      vec4 diffuse=max(dot(normalize(Normal),lightDirection),0.f)*vec4(1,1,1,1);
      color=(ambient+diffuse)*color;
    }
  }


  FragColor = color;
}