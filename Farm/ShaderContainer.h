//----------------------------------------------------------------------------------------
/**
 * \file       ShaderContainer.h
 * \author     Plotnikau Pavel
 * \date       2021/05/20
 * \brief      Defines class which contains shader program and allows us interact with it
*/
//----------------------------------------------------------------------------------------
#ifndef SHADER_CONTAINER_H
#define SHADER_CONTAINER_H

#include "pgr.h"

class ShaderContainer 
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	/// <returns></returns>
	ShaderContainer();
	/// <summary>
	/// Constructor. Gets array of loaded shaders indeces (vs, fs, gs) and creates shader program
	/// </summary>
	/// <param name="shaders"></param>
	/// <returns></returns>
	ShaderContainer(const GLuint* shaders);
	/// <summary>
	/// Returns attribute value index in shader program
	/// </summary>
	/// <param name="name">Name of the attribute value</param>
	/// <returns></returns>
	const GLint GetAttribValue(const char* name);
	/// <summary>
	/// Returns uniform value index in shader program
	/// </summary>
	/// <param name="name">Name of the uniform value</param>
	/// <returns></returns>
	const GLint GetUniformValue(const char* name);
	/// <summary>
	/// Returns shader program
	/// </summary>
	/// <returns></returns>
	const GLuint GetProgram();
	/// <summary>
	/// Sets shader program
	/// </summary>
	/// <param name="program"></param>
	void SetProgram(const GLuint& program);
	/// <summary>
	/// Execute glUseProgram function
	/// </summary>
	void UseProgram();
	/// <summary>
	/// Gets array of loaded shaders indeces (vs, fs, gs) and creates shader program
	/// </summary>
	/// <param name="shaders"></param>
	void CreateProgram(const GLuint* shaders);
	/// <summary>
	/// Copy shader program from anoter shader
	/// </summary>
	/// <param name="another_shader"></param>
	void Copy(const ShaderContainer& another_shader);
private:
	GLuint shader_program;
};

#endif // !SHADER_CONTAINER