#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// uniform sampler2D Texture;

uniform sampler2D texture_base;
uniform sampler2D texture_metalic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_emisive;
uniform sampler2D texture_occlusion;
uniform sampler2D texture_normal;



void main()
{
  FragColor = texture(texture_base, TexCoords) + texture(texture_emisive,TexCoords);
}