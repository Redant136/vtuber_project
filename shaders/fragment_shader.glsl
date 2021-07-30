#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// uniform sampler2D Texture;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_normal1;
uniform sampler2D texture_normal2;
uniform sampler2D texture_normal3;
uniform sampler2D texture_height1;
uniform sampler2D texture_height2;
uniform sampler2D texture_height3;


void main()
{
  FragColor = texture(texture_diffuse1, TexCoords);
  // FragColor = texture(Texture, TexCoords);

}