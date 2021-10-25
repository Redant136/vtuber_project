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
[x] (vrm) fix face (enable blend when necessary)
[x] (extensions) load VRM
[x] (vrm) unlit shader, only add extension (easy)
[x] (loading) warning if file has unexpexted value
[x] (glb rendering) normal texture
[x] (vrm) map materialProperties to material
[x] (rendering) camera in vmodel
[x] (rendering) lauch2(void)
[x] (shader) https://learnopengl.com/Advanced-Lighting/Deferred-Shading (yes, but actually no)
[x] (vrm) mtoon shader
[x] (vrm) mtoon https://www.slideshare.net/VirtualCast/vrm-mtoon HELP!!!
[x] (camera) get camera
[x] (shader) defered rendering is shit, remove
[x] (rendering) DONT BE LAZY, FINISH
[x] (housekeeping) shader do be looking kinda jank, pls fix

## working on
[ ] (optimization) optimize the hell out of draw
[ ] (shader) shadows (hard)


## upcoming
[ ] (face recognition) nn
[ ] (face recognition) tensorflow test
[ ] (camera) https://stackoverflow.com/questions/12483541/webcam-access-in-c-without-opencv
[ ] (camera) libav open camera
[ ] (physics) inverse kinematic
[ ] (physics) find out how unity humanoid functions
[ ] (rendering) get camera pos from gltf
[ ] (housekeeping) gltf buffer mem leak
[ ] (housekeeping) extensions mem leak

[ ] (extensions) parse extras
[ ] (morph) extras.targetNames specify a morph target
[ ] (optimization) morph could be passed to gpu (maybe look into geometry shader)
[ ] (shader) color attribute

[ ] (rendering) use vrm material render queue
[ ] (rendering) switch shader mid draw
[ ] (extensions) consider making vrm MaterialProperties properties a dictionnary instead of static object

## later
[ ] (rendering) finish vrm lighting
[ ] (debug) damaged helmet textures is distorted after a while
[ ] (vrm) vrm 1.0 also called VRMC (VRMC_materials_mtoon, VRMC_materials_hdr_emissiveMultiplier, VRMC_springBone, VRMC_node_constraint)
[ ] (rendering) order rendering stuff, causes clipping (works now for some reason???) http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-10-transparency/
[ ] (rendering) specific UV(texCoord) per texture
[ ] (animations) max and min in accessors
[ ] (animations) interpolation cubic spline

## wants
[ ] (shader) use geometry shader to generate outline
[ ] (shader) spherical harmonics lighting https://catlikecoding.com/unity/tutorials/rendering/part-5/
[ ] (rendering) occlusion texture
[ ] (rendering) roughness texture
[ ] (research) https://github.com/netmail-open/wjelement , https://github.com/petehug/wjelement-cpp/wiki
[ ] (rendering) https://learnopengl.com/Advanced-OpenGL/Instancing
[ ] (rendering) pbr rendering
[ ] (lighting) blinn-phong lighting https://learnopengl.com/Advanced-Lighting/Advanced-Lighting 

## quality of life
[ ] (workflow) remedybg debugger
[ ] (workflow) 4coder
[~] (utils) c utils https://en.cppreference.com/w/c/variadic https://stackoverflow.com/questions/58900903/what-is-compile-time-encapsulation-in-c
[-] (math) m4 and matrix multiplications and transforms
[ ] (glb rendering) reflection textures
[ ] (glb rendering) when material uses multiple texcoords ie texcoord_1 (make larger VAO)
[ ] (glb rendering) texture double sided (gl_FrontFacing)
[ ] (gltf loading) load gltf v1

