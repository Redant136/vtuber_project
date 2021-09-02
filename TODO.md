# TODO



## completed
[x] load glb (took lot more effort than explained here, created this afterwards)
[x] (glb rendering) load data to opengl
[x] (glb rendering) render skeletal view
[x] (glb rendering) load texture data to opengl
[x] (glb rendering) render textures
[x] (research) how does one vao per mesh works
[x] (gltf loading) gltf buffers
[x] (animations) morph targets
[x] (housekeeping) check if m4 math is consistent
[x] (workflow) make updateMorph be called after VBO setup and bufferSubData
[x] (animations) weights anim
[x] (animations) TRS anim
[x] (research) quaternions
[x] (animations) real time switching between frames
[x] (animations) interpolation 
[x] (animations) update morph more than pos

## working on


## upcoming
[ ] (test) check matrix math around l.628, in position change in update()
[ ] (test) test animation translate + scale
[ ] (animations) skinning
[ ] (animations) timing
[ ] (animations) reverse kinematic
[ ] (glb loading) extensions/extra glb storage
[ ] (glb rendering) lighting : https://learnopengl.com/Lighting/Colors
[ ] (housekeeping) gltf buffer mem leak

[ ] (animations) max and min in accessors
[ ] (animations) interpolation cubic spline
[ ] (glb rendering) normal texture
[ ] (glb rendering) occlusion texture
[ ] (glb rendering) roughness texture
[ ] (research) https://github.com/netmail-open/wjelement , https://github.com/petehug/wjelement-cpp/wiki

## wants
[ ] (gltf loading) textures that are not bfviews
[ ] (glb rendering) https://learnopengl.com/Advanced-OpenGL/Instancing

## quality of life
[ ] (glb rendering) reflection textures
[ ] (glb rendering) when material uses multiple texcoords ie texcoord_1 (make larger VAO)
[ ] (glb rendering) texture double sided (gl_FrontFacing)
[ ] (gltf loading) load gltf v1

