#include "computeShader.h"


ComputeShader::ComputeShader()
{
}


ComputeShader::~ComputeShader()
{
}


void ComputeShader::InitShader(const char *filename)
{
	this->LoadShader(filename);
	this->LinkProgram();
	this->UseProgram();
	this->GetUniformHandles();
}


void ComputeShader::UseProgram()
{
	glUseProgram(this->program);
}


// Execute the shader code
void ComputeShader::Draw(int groupSizeX, int groupSizeY)
{
	this->UseProgram();

	glDispatchCompute((GLuint)groupSizeX, (GLuint)groupSizeY, 1);

	// Make sure writing to image has finnished before moving on
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}


// Load Compute shader from file
void ComputeShader::LoadShader(const char *filename)
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
		std::cout << "Error reading compute shader\n";

	const GLchar* shaderString = contents.c_str();

	// Create fragmentShader
	this->shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(this->shader, 1, &shaderString, NULL);
	glCompileShader(this->shader);

	// Get error log
	glGetShaderiv(this->shader, GL_INFO_LOG_LENGTH, &shaderLogSize);
	if (shaderLogSize > 0)
	{
		GLchar* buf = new GLchar[shaderLogSize];
		glGetShaderInfoLog(this->shader, shaderLogSize, NULL, buf);
		printf("[COMPUTE SHADER COMPILE ERROR]: %s\n", buf);
		delete[] buf;
	}


}

// Link the shaders and compile the program
void ComputeShader::LinkProgram()
{
	this->program = glCreateProgram();
	glAttachShader(this->program, this->shader);
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

// Save all uniform locations for quicker access
void ComputeShader::GetUniformHandles()
{
	this->UseProgram();

	GLuint handle;

	// Camera position
	handle = glGetUniformLocation(program, "eye");
	uniformLocations.insert(std::pair<std::string, GLuint>("eye", handle));

	// View frustrum
	handle = glGetUniformLocation(program, "ray00");
	uniformLocations.insert(std::pair<std::string, GLuint>("ray00", handle));
	handle = glGetUniformLocation(program, "ray10");
	uniformLocations.insert(std::pair<std::string, GLuint>("ray10", handle));
	handle = glGetUniformLocation(program, "ray01");
	uniformLocations.insert(std::pair<std::string, GLuint>("ray01", handle));
	handle = glGetUniformLocation(program, "ray11");
	uniformLocations.insert(std::pair<std::string, GLuint>("ray11", handle));
}


void ComputeShader::BindShaderData(std::string name, GLuint index)
{
	glShaderStorageBlockBinding(this->program, uniformLocations[name], index);
}

// Modify uniform int
void ComputeShader::ModifyInt(std::string name, const int i)
{
	GLuint intLocation = uniformLocations[name];
	glUniform1i(intLocation, i);
}

// Modify uniform vec3
void ComputeShader::ModifyVector(std::string name, const Vec3 &v)
{
	GLuint vectorLocation = uniformLocations[name];

	float *arr = new float[3];
	Vec3::GetArray(v, arr);

	glUniform3fv(vectorLocation, 1, arr);

	delete[] arr;
}