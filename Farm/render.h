//----------------------------------------------------------------------------------------
/**
 * \file       render.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Defines initializing and drawing functions 
*/
//----------------------------------------------------------------------------------------
#ifndef RENDER
#define RENDER

#include "pgr.h"
#include "data_parser.h"
#include "ModelContainer.h"
#include "ShaderContainer.h"

/// <summary>
/// Defines position, rotation and scale of any object
/// </summary>
struct Transform {
	glm::vec3 position;
	glm::vec4 rotation;
	glm::vec3 scale;
};

/// <summary>
/// This struct allows to contain simple geometry(like plane) with custom shader
/// </summary>
struct Geometry {
	GLuint VAO, VBO, EBO;
	unsigned int EBO_size;
	ShaderContainer shader;
};

/// <summary>
/// Types of animated textures
/// </summary>
enum AnimTexType {
	UFO,
	FIRE,
	CHARACTER
};

/// <summary>
/// Loads all the data, needs to build the scene
/// </summary>
/// <param name="config_file_path">Path to the config file</param>
void LoadData(const std::string& config_file_path);
/// <summary>
/// Loads all shaders the program need
/// </summary>
/// <returns>Returns true if loading was successfu. Otherwise returns false</returns>
bool LoadShaders();
/// <summary>
/// Loads one hsader program
/// </summary>
/// <param name="vs_path">Path to the vertex shader</param>
/// <param name="fs_path">Path to the fragment shader</param>
/// <param name="program">Returned index of shader progam</param>
/// <returns>Returns true if loading was successfu. Otherwise returns false</returns>
bool LoadSingleShaderProgram(const char* vs_path, const char* fs_path, GLuint& program);
/// <summary>
/// Loads all the data for rendering the skybox 
/// </summary>
void initSkyboxGeometry();
/// <summary>
/// Loads fog texture
/// </summary>
void LoadFogTexture();
/// <summary>
/// Loads all animated textures and oject to render them
/// </summary>
void LoadAnimTextures();
/// <summary>
/// Loads banner texture and oject to render it
/// </summary>
void LoadBanner();
/// <summary>
/// Loads simple model with custom shader
/// </summary>
/// <param name="model_geometry">Returned model</param>
/// <param name="verts">Models vertices</param>
/// <param name="indexes">Models indeces</param>
/// <param name="shader_program">Custom shader</param>
void LoadGeometry(Geometry& model_geometry, float* verts, GLuint* indexes, GLuint shader_program);
/// <summary>
/// Loads textures to buffer
/// </summary>
/// <param name="textures_data">Buffer of loaded textures paths</param>
/// <param name="diffuse">If true saves testures to diffuse textures buffer. Otherwise saves them to specular textures buffer</param>
/// <returns>Returns true if loading was successfu. Otherwise returns false</returns>
bool LoadTextures(const std::vector<std::string>& textures_data, bool diffuse);
/// <summary>
/// Loads models to buffer
/// </summary>
/// <param name="models_data">Buffer of paths to models and texture indeces</param>
/// <returns>Returns true if loading was successfu. Otherwise returns false</returns>
bool LoadModels(const std::vector<std::string>& models_data);
/// <summary>
/// Load campfire
/// </summary>
/// <param name="model">ModelContainer object to save campfire data</param>
void LoadCampfire(ModelContainer** model);
/// <summary>
/// Loads all objects on the scene with their Transform
/// </summary>
/// <param name="objects_data">Buffer of object data</param>
/// <returns>Returns true if loading was successfu. Otherwise returns false</returns>
bool LoadObjects(const std::vector<std::string>& objects_data);
/// <summary>
/// Loads animated object
/// </summary>
void LoadAnimatedObject();
/// <summary>
/// Draws all objects on the scene
/// </summary>
/// <param name="direct_light">Direct light data</param>
/// <param name="point_light">Point light data</param>
/// <param name="slot_light">Spot light data</param>
/// <param name="camera">Camera data</param>
/// <param name="win_width">Window width</param>
/// <param name="win_height">Window height</param>
/// <param name="dt">Delta time</param>
void Draw(const DirectLight& direct_light, const PointLight& point_light, const SpotLight& slot_light, const Camera& camera, GLfloat win_width, GLfloat win_height, float dt);
/// <summary>
/// Draws skybox
/// </summary>
/// <param name="viewMatrix">Matrix of view</param>
/// <param name="projectionMatrix">Matrix of projection</param>
void drawSkybox(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
/// <summary>
/// Draws animated object
/// </summary>
/// <param name="direct_light">Direct light data</param>
/// <param name="point_light">Point light data</param>
/// <param name="slot_light">Spot light data</param>
/// <param name="camera">Camera data</param>
/// <param name="viewMatrix">Matrix of view</param>
/// <param name="projectionMatrix">Matrix of projection</param>
/// <param name="dt">Delta time</param>
void DrawAnimatedObject(const DirectLight& direct_light, const PointLight& point_light, const SpotLight& spot_light, const Camera& camera, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float dt);
/// <summary>
/// Draws banner
/// </summary>
/// <param name="projectionMatrix">Matrix of projection</param>
/// <param name="viewMatrix">Matrix of view</param>
/// <param name="camera">Camera data</param>
/// <param name="dt">Delta time</param>
/// <param name="position">Banner position</param>
/// <param name="scale">Banner scale</param>
void DrawBanner(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, float dt, glm::vec3 position, glm::vec3 scale);
/// <summary>
/// Draws animated texture
/// </summary>
/// <param name="projectionMatrix">Matrix of projection</param>
/// <param name="viewMatrix">Matrix of view</param>
/// <param name="camera">Camera data</param>
/// <param name="type">Type(index) of animated texture in buffer</param>
/// <param name="size_x">Number of columns in texture</param>
/// <param name="size_y">Number of lines in texture</param>
/// <param name="index">Index of textures part (from left to right, then from bottom to top)</param>
/// <param name="position">Position of texture on the scene</param>
/// <param name="scale">Scale of texture</param>
/// <param name="enable_rotation">If true texture will start to look ta the camera</param>
void DrawAnimTexture(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, GLuint type, int size_x, int size_y, int index, glm::vec3 position, glm::vec3 scale, bool enable_rotation = true);
/// <summary>
/// Draws text message on the scene
/// </summary>
/// <param name="projectionMatrix">Matrix of projection</param>
/// <param name="viewMatrix">Matrix of view</param>
/// <param name="camera">Camera data</param>
/// <param name="mes">Message which will be written on the scene</param>
void DrawMessage(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const Camera& camera, std::string mes);
/// <summary>
/// Sends message to the console 
/// </summary>
/// <param name="message">Message which will be send to the console</param>
void LoadFail(const std::string& message);
/// <summary>
/// Returns animated object position and moving diraction
/// </summary>
/// <param name="position">Returned position of animated object</param>
/// <param name="direction">Returned moving diraction of animated object</param>
void GetAnimObjectData(glm::vec3& position, glm::vec3& direction);
/// <summary>
/// Returns campfire position
/// </summary>
/// <param name="position">Returned position of campfire</param>
void GetCampfireData(glm::vec3& position);
/// <summary>
/// Returns model matrix locking in some direction
/// </summary>
/// <param name="direction">Direction in which object is looking</param>
/// <param name="position">Object world position</param>
/// <param name="scale">Object scale</param>
/// <returns>Returns model matrix</returns>
glm::mat4& GetRotatedModelMatrix(const glm::vec3& direction, const glm::vec3& position, const glm::vec3& scale);
/// <summary>
/// Set night intensivity
/// </summary>
/// <param name="val">value from 0.0f to 1.0f</param>
void SetNightValue(float val);
/// <summary>
/// Activate/diactivate fog
/// </summary>
void SwitchFog();
/// <summary>
/// Activate/diactivate campfire
/// </summary>
bool SwitchCampfire();
/// <summary>
/// Activate/diactivate moving of the animated object
/// </summary>
void SwitchAnimObject();
/// <summary>
/// Clears all data
/// </summary>
void ClearData();

#endif // !RENDER