#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec4 a_tangent;
layout (location = 3) in vec2 a_texCoords_0;
layout (location = 4) in vec2 a_texCoords_1;
layout (location = 5) in vec2 a_texCoords_2;
layout (location = 6) in vec4 a_color_0;
layout (location = 7) in vec4 a_joints;
layout (location = 8) in vec4 a_weights;
layout (location = 9) in float a_isOutLine;

// TODO(ANT) vrm attributes

uniform int texCoordIndex;

out vec3 Pos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Tangent;
out vec4 Color;
out vec3 BitTangent;
out float IsOutLine;

uniform mat4 u_jointMatrix[MAX_JOINT_MATRIX];// joint matrices

uniform mat4 node;// specific node transform

uniform mat4 model;// global model transform
uniform mat4 view;// camera
uniform mat4 projection;// camera

void main()
{
    if(texCoordIndex==0){
        TexCoords=a_texCoords_0;
    }else if(texCoordIndex==1){
        TexCoords=a_texCoords_1;
    }else if(texCoordIndex==2){
        TexCoords=a_texCoords_2;
    }
    TexCoords = a_texCoords_0;
    Normal = a_normal;

    mat4 skinMatrix = mat4(1.f);

    if(a_joints.x<u_jointMatrix.length()&&a_joints.y<u_jointMatrix.length()&&a_joints.z<u_jointMatrix.length()&&a_joints.w<u_jointMatrix.length())
    {
        skinMatrix=
            a_weights.x*u_jointMatrix[int(a_joints.x)]+
            a_weights.y*u_jointMatrix[int(a_joints.y)]+
            a_weights.z*u_jointMatrix[int(a_joints.z)]+
            a_weights.w*u_jointMatrix[int(a_joints.w)];
        if(skinMatrix == mat4(0.f))
        {
            skinMatrix = mat4(1.f);
        }
    }
    vec4 pos = projection * view * model * node * skinMatrix * vec4(a_pos, 1.0);
    Pos = vec3(pos);
    gl_Position = pos;

}