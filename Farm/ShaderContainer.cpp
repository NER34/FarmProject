#include<iostream>
#include "ShaderContainer.h"

ShaderContainer::ShaderContainer() : shader_program(0) {}

ShaderContainer::ShaderContainer(const GLuint* shaders) 
{
	shader_program = pgr::createProgram(shaders);
}

const GLint ShaderContainer::GetAttribValue(const char* name) {
	if (shader_program == 0)
		return -1;
	return glGetAttribLocation(shader_program, name);
}

const GLint ShaderContainer::GetUniformValue(const char* name) {
	if (shader_program == 0)
		return -1;
	return glGetUniformLocation(shader_program, name);
}

const GLuint ShaderContainer::GetProgram() {
	return shader_program;
}

void ShaderContainer::SetProgram(const GLuint& program) {
	shader_program = program;
}

void ShaderContainer::UseProgram(){
	glUseProgram(shader_program);
}

void ShaderContainer::CreateProgram(const GLuint* shaders) {
	shader_program = pgr::createProgram(shaders);
}

void ShaderContainer::Copy(const ShaderContainer& another_shader) {
	shader_program = another_shader.shader_program;
}