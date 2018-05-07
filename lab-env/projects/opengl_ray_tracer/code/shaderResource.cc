#include "shaderResource.h"


ShaderResource::ShaderResource()
{

}

ShaderResource::~ShaderResource()
{

}

/**
	Load shaders, link and activate program, and get all uniform handles
*/
void ShaderResource::SetShader(const char *vertexFilename, const char *fragmentFilename)
{
	this->LoadVertexShader(vertexFilename);
	this->LoadFragmentShader(fragmentFilename);
	this->LinkProgram();
	this->GetUniformHandles();
}

/**
	Set the shader program as the active one
*/
void ShaderResource::UseProgram()
{
	glUseProgram(this->program);
}


/**
	Load vertex shader from file
*/
void ShaderResource::LoadVertexShader(const char *filename)
{
	std::ifstream in(filename);
	std::string contents;

	if (in.is_open())
	{
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
	}
	else
		std::cout << "Error reading vertex shader\n";
	
	const GLchar* shaderString = contents.c_str();

	// Create vertexShader
	this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(this->vertexShader, 1, &shaderString, NULL);
	glCompileShader(this->vertexShader);

	// Get error log
	glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
		printf("[VERTEX SHADER COMPILE ERROR]: %s\n", buf);
		delete[] buf;
	}
}

/**
	Load fragment shader from file
*/
void ShaderResource::LoadFragmentShader(const char *filename)
{
	std::ifstream in(filename);
	std::string contents;

	if (in.is_open())
	{
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
	}
	else
		std::cout << "Error reading fragment shader\n";

	const GLchar* shaderString = contents.c_str();

	// Create fragmentShader
	this->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(this->fragmentShader, 1, &shaderString, NULL); //&length
	glCompileShader(this->fragmentShader);

	// Get error log
	glGetShaderiv(this->fragmentShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(this->fragmentShader, shaderLogSize, NULL, buf);
		printf("[FRAGMENT SHADER COMPILE ERROR]: %s\n", buf);
		delete[] buf;
	}
}

/**
	Link the shaders to a program
*/
void ShaderResource::LinkProgram()
{
	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertexShader);
	glAttachShader(this->program, this->fragmentShader);
	glLinkProgram(this->program);

	// Get any error
	glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
		printf("[PROGRAM LINK ERROR]: %s\n", buf);
		delete[] buf;
	}
}

/**
	Save all uniform locations for quicker access
*/
void ShaderResource::GetUniformHandles()
{
	this->UseProgram();

	GLuint handle;

	// Camera and model
	handle = glGetUniformLocation(program, "MVP");
	uniformLocations.insert(std::pair<std::string, GLuint>("MVP", handle));
	handle = glGetUniformLocation(program, "V");
	uniformLocations.insert(std::pair<std::string, GLuint>("V", handle));
	handle = glGetUniformLocation(program, "M");
	uniformLocations.insert(std::pair<std::string, GLuint>("M", handle));


	// Texture
	handle = glGetUniformLocation(program, "diffuseTextureSampler");
	uniformLocations.insert(std::pair<std::string, GLuint>("diffuseTextureSampler", handle));
}


/**
	Modify uniform mat4
*/
void ShaderResource::ModifyMatrix(std::string name, const Matrix &m)
{
	GLuint matrixLocation = uniformLocations[name];

	float *arr = new float[16];
	Matrix::GetArray(m, arr);

	glUniformMatrix4fv(matrixLocation, 1, GL_TRUE, arr);

	delete[] arr;
}
/**
	Modify uniform vec3
*/
void ShaderResource::ModifyVector(std::string name, const Vec3 &v)
{
	float *arr = new float[3];
	Vec3::GetArray(v, arr);

	GLuint vectorLocation = uniformLocations[name];
	glUniform3fv(vectorLocation, 1, arr);

	delete[] arr;
}
/**
	Modify uniform float
*/
void ShaderResource::ModifyFloat(std::string name, const float f)
{
	GLuint floatLocation = uniformLocations[name];
	glUniform1f(floatLocation, f);
}
/**
	Modify uniform int
*/
void ShaderResource::ModifyInt(std::string name, const int i)
{
	GLuint intLocation = uniformLocations[name];
	glUniform1i(intLocation, i);
}