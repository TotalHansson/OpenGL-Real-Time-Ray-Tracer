#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <string>

#ifndef GL_INCLUDED
#define GL_INCLUDED
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "mathMatrix.h"
#include "mathVec3.h"


class ShaderResource
{
public:
	ShaderResource();
	~ShaderResource();

	// Load shaders and link the program
	void SetShader(const char* vertexFilename, const char* fragmentFilename);
	void UseProgram();

	// Modify uniforms with handle names
	void ModifyMatrix(std::string name, const Matrix &m);
	void ModifyVector(std::string name, const Vec3 &v);
	void ModifyFloat(std::string name, const float f);
	void ModifyInt(std::string name, const int i);


private:
	std::string vsString;
	std::string fsString;

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	GLint shaderLogSize;

	std::map<std::string, GLuint> uniformLocations;


	void LoadVertexShader(const char* filename);
	void LoadFragmentShader(const char* filename);
	void LinkProgram();
	void GetUniformHandles();
};