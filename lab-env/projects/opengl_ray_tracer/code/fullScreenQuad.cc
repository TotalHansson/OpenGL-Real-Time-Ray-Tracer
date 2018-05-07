#include "fullScreenQuad.h"


FullScreenQuad::FullScreenQuad()
{
	// Quad
	GLfloat buf[] =
	{
		-1.0f,	-1.0f,	-1,		// BL
		 0.0f,	 0.0f,
		-1.0f,	 1.0f,	-1,		// TL
		 0.0f,	 1.0f,
		 1.0f,	-1.0f,	-1,		// BR
		 1.0f,	 0.0f,

		-1.0f,	 1.0f,	-1,		// TL
		 0.0f,   1.0f,
		 1.0f,	 1.0f,	-1,		// TR
		 1.0f,   1.0f,
		 1.0f,	-1.0f,	-1,		// BR
		 1.0f,   0.0f,
	};

	// Setup VBO
	glGenBuffers(1, &this->quad);
	glBindBuffer(GL_ARRAY_BUFFER, this->quad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buf), buf, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Set shader for the quad
	this->shader = new ShaderResource;
	this->shader->SetShader("../resources/shaders/basic.vertexshader", "../resources/shaders/basic.fragmentshader");
}


FullScreenQuad::~FullScreenQuad()
{
	delete this->shader;
}


// Render a quad from (-1,-1) to (1,1) using the texture bound to id "frameBuffer"
void FullScreenQuad::Draw(GLuint frameBuffer)
{
	this->shader->UseProgram();

	// Activate texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, frameBuffer);

	// Draw the quad
	glBindBuffer(GL_ARRAY_BUFFER, this->quad);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (GLvoid*)(sizeof(float) * 3));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}