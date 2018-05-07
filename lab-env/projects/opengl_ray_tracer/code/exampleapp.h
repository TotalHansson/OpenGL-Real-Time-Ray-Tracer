#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"

#include "fullScreenQuad.h"
#include "computeShader.h"
#include "gameObject.h"
#include "lineSweep.h"
#include "GJK.h"
#include "EPA.h"
#include "camera.h"
#include "objParser.h"

#include <vector>
#include <chrono>
#include <string>

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open();
	/// run app
	void Run();
private:

	void RenderUI();
	void CreateObjects();
	void SendBuffer(std::vector<GameObject*> &go, GLuint ssbo, bool isStatic);

	Display::Window* window;

	// Controls
	bool lClicking = false;
	bool rClicking = false;
	bool mouseLock = true;
	float oldMouseX = 0;
	float oldMouseY = 0;
	float mouseSpeed = 0.1f;


	// Camera
	Camera camera;

	// Quad covering the screen that the ray traced image will be rendered to
	FullScreenQuad *quad = nullptr;

	// Environment
	GLuint staticSSBO, dynamicSSBO;
	std::vector<GameObject*> staticGO;
	std::vector<GameObject*> dynamicGO;

	// Compute Shader
	ComputeShader *computeShader = nullptr;
	int texWidth, texHeight;
	GLuint frameBuffer;


	float dt = 0; 		// Total frame time
	std::chrono::time_point<std::chrono::system_clock> start, end;
	long long counter = 0;
};
} // namespace Example