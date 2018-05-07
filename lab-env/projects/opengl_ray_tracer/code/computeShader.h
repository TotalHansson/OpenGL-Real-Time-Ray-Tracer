#pragma once

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifndef GL_INCLUDED
#define GL_INCLUDED
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include "mathVec3.h"

class ComputeShader
{
public:
	ComputeShader();
	~ComputeShader();

	void InitShader(const char *filename);
	void UseProgram();
	void Draw(int texWidth, int texHeight);

	void BindShaderData(std::string name, GLuint index);
	void ModifyInt(std::string name, const int i);
	void ModifyVector(std::string name, const Vec3 &v);
	void ModifyMatrixArray(std::string name, float *matriceData, int numMatrices);

private:
	GLuint shader;
	GLint shaderLogSize;
	GLuint program;
	std::map<std::string, GLuint> uniformLocations;

	void GetUniformHandles();
	void LoadShader(const char *filename);
	void LinkProgram();
};

