#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

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
    vec2 UV = TexCoords;
    
    vec3 FragPos = texture(gPosition, UV).rgb;
    vec3 Normal = texture(gNormal, UV).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, UV).rgb;
    float Specular = texture(gAlbedoSpec, UV).a;
    
    vec2 texOffset = 1.0 / vec2(textureSize(gNormal, 0).xy);
    vec4 NormalDistanceSquareVec;// x:right,y:left,z:up,w:down
    NormalDistanceSquareVec.x=distanceSquare(Normal,texture(gNormal,UV+texOffset*vec2(+1,0)).xyz);
    NormalDistanceSquareVec.y=distanceSquare(Normal,texture(gNormal,UV+texOffset*vec2(-1,0)).xyz);
    NormalDistanceSquareVec.z=distanceSquare(Normal,texture(gNormal,UV+texOffset*vec2(0,+1)).xyz);
    NormalDistanceSquareVec.w=distanceSquare(Normal,texture(gNormal,UV+texOffset*vec2(0,-1)).xyz);
    float NormalDistanceSquare=dot(NormalDistanceSquareVec,vec4(1,1,1,1));

    vec4 PosDistanceSquareVec;// x:right,y:left,z:up,w:down
    PosDistanceSquareVec.x=distanceSquare(FragPos,texture(gPosition,UV+texOffset*vec2(+1,0)).xyz);
    PosDistanceSquareVec.y=distanceSquare(FragPos,texture(gPosition,UV+texOffset*vec2(-1,0)).xyz);
    PosDistanceSquareVec.z=distanceSquare(FragPos,texture(gPosition,UV+texOffset*vec2(0,+1)).xyz);
    PosDistanceSquareVec.w=distanceSquare(FragPos,texture(gPosition,UV+texOffset*vec2(0,-1)).xyz);
    float PosDistanceSquare=dot(PosDistanceSquareVec,vec4(1,1,1,1));

    // FragColor=vec4(texture(camtex,UV));
    // return;

    if(!(abs(UV.x)>=1||abs(UV.y)>=1)&&PosDistanceSquare*NormalDistanceSquare>0.002){
        FragColor=vec4(0,0,0,1);
        return;
    }

    FragColor = vec4(Diffuse, 1);
}