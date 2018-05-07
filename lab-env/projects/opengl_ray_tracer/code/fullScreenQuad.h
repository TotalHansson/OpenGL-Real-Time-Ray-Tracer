#pragma once

#include "shaderResource.h"


class FullScreenQuad
{
public:
	FullScreenQuad();
	~FullScreenQuad();

	void Draw(GLuint frameBuffer);


	GLuint quad;
	ShaderResource *shader;
};