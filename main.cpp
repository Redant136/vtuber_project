// #include <iostream>
// #include <map>

// #include <GL/glut.h>

// #include <assimp/vector3.h>
// #include <assimp/matrix4x4.h>
// #include <assimp/Importer.hpp>  // C++ importer interface
// #include <assimp/scene.h>       // Output data structure
// #include <assimp/postprocess.h> // Post processing flags
// #include <assimp/DefaultLogger.hpp>

// using namespace std;
// using namespace Assimp;

// const aiScene *scene = NULL;
// Importer importer;
// aiVector3D scene_min, scene_max, scene_center;
// GLuint scene_list = 0;
// GLfloat angle = 0;
// GLuint *textureIds = NULL; // pointer to texture Array
// #define aisgl_min(x, y) (x < y ? x : y)
// #define aisgl_max(x, y) (y > x ? y : x)

// #define Helper_ColorIsAbsent(c) ((c.r == 0) && (c.g == 0) && (c.b == 0) && (c.a == 0))

// void get_bounding_box_for_node(const aiNode *nd, aiVector3D *min, aiVector3D *max, aiMatrix4x4 *trafo)
// {
//   aiMatrix4x4 prev;
//   unsigned int n = 0, t;

//   prev = *trafo;
//   *trafo = *trafo * nd->mTransformation;
//   for (; n < nd->mNumMeshes; ++n)
//   {
//     const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[n]];

//     for (t = 0; t < mesh->mNumVertices; ++t)
//     {
//       aiVector3D tmp = mesh->mVertices[t];

//       tmp *= *trafo;

//       min->x = aisgl_min(min->x, tmp.x);
//       min->y = aisgl_min(min->y, tmp.y);
//       min->z = aisgl_min(min->z, tmp.z);

//       max->x = aisgl_max(max->x, tmp.x);
//       max->y = aisgl_max(max->y, tmp.y);
//       max->z = aisgl_max(max->z, tmp.z);
//     }
//   }

//   for (n = 0; n < nd->mNumChildren; ++n)
//   {
//     get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);
//   }

//   *trafo = prev;
// }

// void get_bounding_box(aiVector3D *min, aiVector3D *max)
// {
//   aiMatrix4x4 trafo;

//   min->x = min->y = min->z = 1e10f;
//   max->x = max->y = max->z = -1e10f;
//   get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
// }

// void LogInfo(const std::string &pMessage)
// {
//   Assimp::DefaultLogger::get()->info(pMessage);
// }

// void LogError(const std::string &pMessage)
// {
//   Assimp::DefaultLogger::get()->error(pMessage);
// }

// bool LoadFromFile(const std::string &pFilename)
// {
//   bool rv = false;

//   LogInfo("importer.ReadFile");
//   scene = importer.ReadFile(pFilename, aiProcess_GenNormals);
//   if (scene)
//   {
//     LogInfo("Import done: " + pFilename);
//     get_bounding_box(&scene_min, &scene_max);
//     scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
//     scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
//     scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
//     rv = true;
//   }
//   else
//   {
//     LogError("Error parsing \'" + pFilename + "\': \'" + importer.GetErrorString() + "\'");
//     rv = false;
//   }

//   return rv;
// }

// void color4_to_float4(const aiColor4D *c, float f[4])
// {
//   f[0] = c->r;
//   f[1] = c->g;
//   f[2] = c->b;
//   f[3] = c->a;
// }

// void set_float4(float f[4], float a, float b, float c, float d)
// {
//   f[0] = a;
//   f[1] = b;
//   f[2] = c;
//   f[3] = d;
// }

// void apply_material(const aiMaterial *mtl)
// {
//   float c[4];
//   int ret1, ret2;
//   aiColor4D diffuse;
//   aiColor4D specular;
//   aiColor4D ambient;
//   aiColor4D emission;
//   float shininess, strength;
//   unsigned int max; // changed: to unsigned

//   set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
//   if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
//     color4_to_float4(&diffuse, c);

//   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

//   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
//   if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
//     color4_to_float4(&specular, c);

//   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

//   set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
//   if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
//     color4_to_float4(&ambient, c);

//   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

//   set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
//   if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
//     color4_to_float4(&emission, c);

//   glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

//   max = 1;
//   ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
//   max = 1;
//   ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
//   if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
//   {
//     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
//   }
//   else
//   {
//     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
//     set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
//     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
//   }

//   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// }

// void recursive_render(const struct aiScene *sc, const struct aiNode *nd)
// {
//   unsigned int i;
//   unsigned int n = 0, t;
//   aiMatrix4x4 m = nd->mTransformation;
//   GLuint prev_tex_id_idx = 0;

//   // update transform
//   m.Transpose();
//   glPushMatrix();
//   glMultMatrixf((float *)&m);
//   // draw all meshes assigned to this node
//   for (; n < nd->mNumMeshes; ++n)
//   {
//     const struct aiMesh *mesh = scene->mMeshes[nd->mMeshes[n]];

//     if (mesh->HasTextureCoords(0))
//     {
//       // enable first texture as default
//       prev_tex_id_idx = 0;
//       glBindTexture(GL_TEXTURE_2D, textureIds[prev_tex_id_idx]);
//     }

//     apply_material(sc->mMaterials[mesh->mMaterialIndex]);
//     if (!mesh->HasTextureCoords(0) && mesh->HasVertexColors(0))
//       glEnable(GL_COLOR_MATERIAL);
//     else
//       glDisable(GL_COLOR_MATERIAL);

//     // draw loop
//     for (t = 0; t < mesh->mNumFaces; ++t)
//     {
//       const struct aiFace *face = &mesh->mFaces[t];
//       GLenum face_mode;

//       switch (face->mNumIndices)
//       {
//       case 1:
//         face_mode = GL_POINTS;
//         break;
//       case 2:
//         face_mode = GL_LINES;
//         break;
//       case 3:
//         face_mode = GL_TRIANGLES;
//         break;
//       default:
//         face_mode = GL_POLYGON;
//         break;
//       }

//       if (mesh->HasTextureCoords(0))
//       {
//         // get current texture ID and check if need to enable new texture
//         if (mesh->mTextureCoords[1][t].x != prev_tex_id_idx)
//         {
//           prev_tex_id_idx = mesh->mTextureCoords[1][t].x;
//           glBindTexture(GL_TEXTURE_2D, textureIds[prev_tex_id_idx]);
//         }
//       }

//       glBegin(face_mode);
//       for (i = 0; i < face->mNumIndices; i++)
//       {
//         const GLfloat vtx_def_col[4] = {0.5f, 0.5f, 0.5f, 1};

//         int index = face->mIndices[i];

//         //
//         // colors
//         //
//         if ((mesh->mColors[1] != NULL) && !Helper_ColorIsAbsent(mesh->mColors[1][t])) // check if color set for face
//           glColor4fv((GLfloat *)&mesh->mColors[1][t]);
//         else if ((mesh->mColors[0] != NULL) && !Helper_ColorIsAbsent(mesh->mColors[0][index])) // check if color set for vertex
//           glColor4fv((GLfloat *)&mesh->mColors[0][index]);
//         else                       // default color for vertex.
//           glColor4fv(vtx_def_col); ///TODO: IME thru AI_*
//         //
//         // textures
//         //
//         if (mesh->HasTextureCoords(0))
//         {
//           glTexCoord2f(mesh->mTextureCoords[0][t * 3 + i].x, 1 - mesh->mTextureCoords[0][t * 3 + i].y); //mTextureCoords[channel][vertex]
//         }
//         //
//         // normals
//         //
//         if (mesh->mNormals != NULL)
//           glNormal3fv(&mesh->mNormals[index].x);

//         glVertex3fv(&mesh->mVertices[index].x);
//       }

//       glEnd();
//     }
//   }

//   // draw all children
//   for (n = 0; n < nd->mNumChildren; ++n)
//     recursive_render(sc, nd->mChildren[n]);

//   glPopMatrix();
// }

// void do_motion(void)
// {
//   static GLint prev_time = 0;
//   static GLint prev_fps_time = 0;
//   static int frames = 0;
//   int time = glutGet(GLUT_ELAPSED_TIME);

//   angle += (time - prev_time) * 0.03;
//   prev_time = time;
//   frames += 1;
//   if ((time - prev_fps_time) > 1000) // update every seconds
//   {
//     int current_fps = frames * 1000 / (time - prev_fps_time);

//     cout << current_fps << " FPS" << endl;
//     frames = 0;
//     prev_fps_time = time;
//   }

//   glutPostRedisplay();
// }

// void reshape(int width, int height)
// {
//   const double aspectRatio = (float)width / height, fieldOfView = 45.0;

//   glMatrixMode(GL_PROJECTION);
//   glLoadIdentity();
//   gluPerspective(fieldOfView, aspectRatio, 1.0, 1000.0); /* Znear and Zfar */
//   glViewport(0, 0, width, height);
// }

// void display(void)
// {
//   float tmp;

//   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//   glMatrixMode(GL_MODELVIEW);
//   glLoadIdentity();
//   gluLookAt(5.f, 0.f, -3.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f);
//   // rotate it around the y axis
//   glRotatef(angle, 0.f, 0.f, 1.f);
//   // scale the whole asset to fit into our view frustum
//   tmp = scene_max.x - scene_min.x;
//   tmp = aisgl_max(scene_max.y - scene_min.y, tmp);
//   tmp = aisgl_max(scene_max.z - scene_min.z, tmp);
//   tmp = 2.0f / tmp;
//   glScalef(tmp, tmp, tmp);
//   // center the model
//   glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);
//   // if the display list has not been made yet, create a new one and
//   // fill it with scene contents
//   if (scene_list == 0)
//   {
//     scene_list = glGenLists(1);
//     glNewList(scene_list, GL_COMPILE);
//     // now begin at the root node of the imported data and traverse
//     // the scenegraph by multiplying subsequent local transforms
//     // together on GL's matrix stack.
//     recursive_render(scene, scene->mRootNode);
//     glEndList();
//   }

//   glCallList(scene_list);
//   glutSwapBuffers();
//   do_motion();
// }

// string getBasePath(const string &path)
// {
//   size_t pos = path.find_last_of("\\/");

//   return (string::npos == pos) ? "" : path.substr(0, pos + 1);
// }

// void LoadGLTextures(const aiScene *scene, const string &pModelPath)
// {
//   // Check if scene has textures.
//   if (scene->HasTextures())
//   {
//     textureIds = new GLuint[scene->mNumTextures];
//     glGenTextures(scene->mNumTextures, textureIds); // generate GL-textures ID's
//     // upload textures
//     for (size_t ti = 0; ti < scene->mNumTextures; ti++)
//     {
//       glBindTexture(GL_TEXTURE_2D, textureIds[ti]); // Binding of texture name
//       //
//       //redefine standard texture values
//       //
//       // We will use linear interpolation for magnification filter
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//       // tiling mode
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (scene->mTextures[ti]->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP);
//       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (scene->mTextures[ti]->achFormatHint[0] & 0x01) ? GL_REPEAT : GL_CLAMP);
//       // Texture specification
//       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, scene->mTextures[ti]->mWidth, scene->mTextures[ti]->mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE,
//                    scene->mTextures[ti]->pcData);
//     }
//   }
// }

// int main(int argc, char **argv)
// {
//   using namespace Assimp;

//   string model_path;

//   DefaultLogger::create("", Logger::VERBOSE, aiDefaultLogStream_DEBUGGER | aiDefaultLogStream_STDOUT);

//   if (argc < 2)
//   {
//     cout << "amftest filename" << endl;

//     goto err_end;
//   }

//   model_path = argv[1];

//   if (!LoadFromFile(model_path))
//     goto err_end;

//   LogInfo("Init GLUT");
//   glutInitWindowSize(1024, 768);
//   glutInitWindowPosition(100, 100);
//   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
//   glutInit(&argc, argv);

//   LogInfo("Create window");
//   glutCreateWindow("Assimp - AMF test");
//   glutDisplayFunc(display);
//   glutReshapeFunc(reshape);

//   glClearColor(0.0f, 0.0f, 0.0f, 1.f);
//   glShadeModel(GL_SMOOTH);
//   glEnable(GL_TEXTURE_2D);
//   glEnable(GL_DEPTH_TEST);
//   glEnable(GL_NORMALIZE);
//   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

//   glEnable(GL_LIGHTING);

//   {
//     GLfloat lightpos[] = {10.0, 10.0, 10.0, 1.0};
//     GLfloat light_ambcolor[] = {1, 1, 1, 1.0};
//     GLfloat light_diffcolor[] = {1.0, 1.0, 1.0, 1.0};
//     GLfloat ambcolor[] = {0.5, 0.5, 0.5, 1.0};

//     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambcolor);
//     glEnable(GL_LIGHT0);
//     glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
//     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambcolor);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffcolor);
//     glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffcolor);
//   }

//   // XXX docs say all polygons are emitted CCW, but tests show that some aren't.
//   if (getenv("MODEL_IS_BROKEN"))
//     glFrontFace(GL_CW);

//   LoadGLTextures(scene, model_path);
//   glutGet(GLUT_ELAPSED_TIME);
//   LogInfo("Main loop");
//   glutMainLoop();
//   LogInfo("Exit");

//   DefaultLogger::kill();
//   if (textureIds)
//     delete[] textureIds;

//   return 0;

// err_end:

//   DefaultLogger::kill();
//   if (textureIds)
//     delete[] textureIds;

//   return (-1);
// }












// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

// #include "learnopengl/shader_m.h"
// #include "learnopengl/camera.h"
// #include "learnopengl/model.h"

// #include <iostream>

// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
// void processInput(GLFWwindow *window);

// // settings
// const unsigned int SCR_WIDTH = 800;
// const unsigned int SCR_HEIGHT = 600;

// // camera
// Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
// float lastX = SCR_WIDTH / 2.0f;
// float lastY = SCR_HEIGHT / 2.0f;
// bool firstMouse = true;

// // timing
// float deltaTime = 0.0f;
// float lastFrame = 0.0f;

// int main()
// {
//   // glfw: initialize and configure
//   // ------------------------------
//   glfwInit();
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// #ifdef __APPLE__
//   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//   // glfw window creation
//   // --------------------
//   GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//   if (window == NULL)
//   {
//     std::cout << "Failed to create GLFW window" << std::endl;
//     glfwTerminate();
//     return -1;
//   }
//   glfwMakeContextCurrent(window);
//   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//   glfwSetCursorPosCallback(window, mouse_callback);
//   glfwSetScrollCallback(window, scroll_callback);

//   // tell GLFW to capture our mouse
//   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

//   // glad: load all OpenGL function pointers
//   // ---------------------------------------
//   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//   {
//     std::cout << "Failed to initialize GLAD" << std::endl;
//     return -1;
//   }

//   // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
//   stbi_set_flip_vertically_on_load(true);

//   // configure global opengl state
//   // -----------------------------
//   glEnable(GL_DEPTH_TEST);

//   // build and compile shaders
//   // -------------------------
//   Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

//   // load models
//   // -----------
//   Model ourModel("models/Avocado.glb");

//   // draw in wireframe
//   //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//   // render loop
//   // -----------
//   while (!glfwWindowShouldClose(window))
//   {
//     // per-frame time logic
//     // --------------------
//     float currentFrame = glfwGetTime();
//     deltaTime = currentFrame - lastFrame;
//     lastFrame = currentFrame;

//     // input
//     // -----
//     processInput(window);

//     // render
//     // ------
//     glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // don't forget to enable shader before setting uniforms
//     ourShader.use();

//     // view/projection transformations
//     glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//     glm::mat4 view = camera.GetViewMatrix();
//     ourShader.setMat4("projection", projection);
//     ourShader.setMat4("view", view);

//     // render the loaded model
//     glm::mat4 model = glm::mat4(1.0f);
//     model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
//     model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
//     ourShader.setMat4("model", model);
//     ourModel.Draw(ourShader);

//     // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//     // -------------------------------------------------------------------------------
//     glfwSwapBuffers(window);
//     glfwPollEvents();
//   }

//   // glfw: terminate, clearing all previously allocated GLFW resources.
//   // ------------------------------------------------------------------
//   glfwTerminate();
//   return 0;
// }

// // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// // ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//     glfwSetWindowShouldClose(window, true);

//   if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//     camera.ProcessKeyboard(FORWARD, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//     camera.ProcessKeyboard(BACKWARD, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//     camera.ProcessKeyboard(LEFT, deltaTime);
//   if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//     camera.ProcessKeyboard(RIGHT, deltaTime);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//   // make sure the viewport matches the new window dimensions; note that width and
//   // height will be significantly larger than specified on retina displays.
//   glViewport(0, 0, width, height);
// }

// // glfw: whenever the mouse moves, this callback is called
// // -------------------------------------------------------
// void mouse_callback(GLFWwindow *window, double xpos, double ypos)
// {
//   if (firstMouse)
//   {
//     lastX = xpos;
//     lastY = ypos;
//     firstMouse = false;
//   }

//   float xoffset = xpos - lastX;
//   float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

//   lastX = xpos;
//   lastY = ypos;

//   camera.ProcessMouseMovement(xoffset, yoffset);
// }

// // glfw: whenever the mouse scroll wheel scrolls, this callback is called
// // ----------------------------------------------------------------------
// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
// {
//   camera.ProcessMouseScroll(yoffset);
// }












#include "vtuber.hpp"
using namespace vtuber;

int main(){
  launch();
  

  return 0;
}