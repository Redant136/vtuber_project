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
[x] (animations) make updateMorph be called after VBO setup and bufferSubData
[x] (animations) weights anim
[x] (animations) TRS anim
[x] (research) quaternions
[x] (animations) real time switching between frames
[x] (animations) interpolation 
[x] (animations) update morph more than pos
[x] (rendering) global transform
[x] (math) v2, v3, v4 lib
[x] (rendering) OPTIMIZATION, YEAH!!! (flag for updates,pass mat4 to gpu)
[x] (animations) skinning
[x] (test) check matrix math around l.628, in TRS + morph in update()
[x] (test) test animation translate + scale
[x] (debug) draw skeleton
[x] (skinning) follow randy dev logs on skinning (not solved prob)
[x] (skinning) figure out how skeleton root node functions (absolutly useless)
[x] (debug) fix skinning
[x] (glb rendering) lighting : https://learnopengl.com/Lighting/Colors
[x] (gltf loading) textures that are not bfviews
[x] (extensions) KHR_materials_unlit
[x] (extensions) KHR_texture_transform
[x] (extensions) desirialize VRM

## working on
[ ] (extensions) VRM
[ ] (vrm rendering) https://github.com/vrm-c/vrm-specification/tree/master/specification/0.0#vrm-rules


## upcoming
[ ] (optimization) morph could be passed to gpu (maybe look into geometry shader)
[ ] (extensions) VRMC_materials_mtoon
[ ] (extensions) VRMC_materials_hdr_emissiveMultiplier
[ ] (extensions) VRMC_springBone
[ ] (extensions) VRMC_node_constraint
[ ] (morph) extras.targetNames specify a morph target

[ ] (animations) reverse kinematic
[ ] (housekeeping) gltf buffer mem leak
[ ] (housekeeping) extensions mem leak
[ ] (loading) warning if file has unexpexted value

[ ] (rendering) specific UV(texCoord) per texture
[ ] (animations) max and min in accessors
[ ] (animations) interpolation cubic spline
[ ] (glb rendering) normal texture
[ ] (glb rendering) occlusion texture
[ ] (glb rendering) roughness texture
[ ] (research) https://github.com/netmail-open/wjelement , https://github.com/petehug/wjelement-cpp/wiki

## wants
[ ] (glb rendering) https://learnopengl.com/Advanced-OpenGL/Instancing
[ ] (rendering) pbr rendering
[ ] (workflow) remedybg debugger
[ ] (workflow) 4coder

## quality of life
[ ] (lighting) blinn-phong lighting https://learnopengl.com/Advanced-Lighting/Advanced-Lighting 
[-] (math) m4 and matrix multiplications and transforms
[ ] (glb rendering) reflection textures
[ ] (glb rendering) when material uses multiple texcoords ie texcoord_1 (make larger VAO)
[ ] (glb rendering) texture double sided (gl_FrontFacing)
[ ] (gltf loading) load gltf v1

