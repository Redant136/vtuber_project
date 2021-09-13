#version 330 core
#define MAX_JOINT_MATRIX 256

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texCoords;
layout (location = 3) in vec4 a_joints;
layout (location = 4) in vec4 a_weights;

out vec2 TexCoords;

uniform mat4 u_jointMatrix[MAX_JOINT_MATRIX];

uniform mat4 node;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = a_texCoords;

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
    gl_Position = projection * view * model * node * skinMatrix * vec4(a_pos, 1.0);

}