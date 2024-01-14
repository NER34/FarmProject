//----------------------------------------------------------------------------------------
/**
 * \file       ModelContainer.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Defines class which contains all information about models and allows us to draw it on the scene
*/
//----------------------------------------------------------------------------------------
#ifndef MODEL_CONTAINER_H
#define MODEL_CONTAINER_H

#include "pgr.h"
#include "ShaderContainer.h"
#include "LightSourses.h"
#include "CameraContainer.h"

class ModelContainer 
{
public: 
	/// Destructor
	~ModelContainer();
	/// <summary>
	/// Initialize model
	/// </summary>
	/// <param name="path">Path to model in file system</param>
	/// <param name="shader_program"></param>
	/// <param name="stencil_id">Model id</param>
	/// <param name="transform_model">Allows to change model geometry in vertex shader</param>
	/// <param name="glass_mode">Makes the object transparent</param>
	/// <returns>Returns true if loading was succesful. Otherwise returns false</returns>
	bool CreateModel(const char* path, const GLuint& shader_program, const GLbyte& stencil_id = 0, const bool& transform_model = false, const bool& glass_mode = false);
	/// <summary>
	/// Sets model material
	/// </summary>
	/// <param name="diffuse_texture_path">Path to diffuse texture in file system</param>
	/// <param name="specular_texture_path">Path to specular texture in file system</param>
	/// <param name="shininess"></param>
	/// <returns>Returns true if loading was succesful. Otherwise returns false</returns>
	bool SetMaterial(const char* diffuse_texture_path, const char* specular_texture_path, float shininess);
	/// <summary>
	/// Sets model material
	/// </summary>
	/// <param name="diffuse_texture">Diffuse texture id</param>
	/// <param name="specular_texture">Specular texture id</param>
	/// <param name="shininess"></param>
	void SetMaterial(const GLuint& diffuse_texture, const GLuint& specular_texture, float shininess);
	/// <summary>
	/// Sets shader program
	/// </summary>
	/// <param name="shader_program"></param>
	void SetShaderProgram(GLuint shader_program);
	/// <summary>
	/// </summary>
	/// <returns>Returns shader</returns>
	ShaderContainer& GetShader();
	/// <summary>
	/// Sets Vertex Array Object id
	/// </summary>
	/// <param name="vao"></param>
	void SetVAO(GLuint vao);
	/// <summary>
	/// Sets Vertex Buffer Object id
	/// </summary>
	/// <param name="vbo"></param>
	void SetVBO(GLuint vbo);
	/// <summary>
	/// Sets Element Buffer Object id
	/// </summary>
	/// <param name="ebo"></param>
	/// <param name="ebo_size">Size of the buffer</param>
	void SetEBO(GLuint ebo, unsigned int ebo_size);
	/// <summary>
	/// Sets fog texturre id
	/// </summary>
	/// <param name="texture"></param>
	void SetFogTexture(GLuint texture);
	/// <summary>
	/// Sets model id
	/// </summary>
	/// <param name="_stencil_id"></param>
	void SetStencilId(const GLbyte& _stencil_id);
	/// <summary>
	/// Draw model on the scene
	/// </summary>
	/// <param name="modelMatrix"></param>
	/// <param name="viewMatrix"></param>
	/// <param name="projectionMatrix"></param>
	/// <param name="direct">Direct light data</param>
	/// <param name="point">Point light data</param>
	/// <param name="slot">Spot light data</param>
	/// <param name="camera">Camera data</param>
	/// <param name="dt">Delta time</param>
	/// <param name="fog_enabled">Draw fog texture on the model if fog is enabled</param>
	void Draw(const glm::mat4& modelMatrix, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, DirectLight direct, PointLight point, SpotLight spot, Camera camera, float dt, bool fog_enabled);
private:
	/// <summary>
	/// Defines information about one vertex
	/// </summary>
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coords;
	};
	/// <summary>
	/// Defines models material
	/// </summary>
	struct Material {
		GLuint diffuse_texture;
		GLuint specular_texture;
		float shininess;
	};

	GLuint VAO, VBO, EBO;
	unsigned int EBO_size;
	Material material;
	GLuint fog_texture;
	ShaderContainer shader;
	GLbyte stencil_id;
	bool transform_model;
	bool glass_mode;
	float time;
};

#endif