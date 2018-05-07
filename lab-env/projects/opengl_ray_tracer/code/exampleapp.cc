//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include "imgui.h"

#ifndef GL_INCLUDED
#define GL_INCLUDED
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

#include <cstring>
#include <stdlib.h>

using namespace Display;
namespace Example
{

//------------------------------------------------------------------------------
/**
*/
ExampleApp::ExampleApp()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ExampleApp::~ExampleApp()
{
	// empty
}


//------------------------------------------------------------------------------
/**
*/
bool
ExampleApp::Open()
{
	App::Open();
	this->window = new Display::Window;
	window->SetKeyPressFunction([this](int key, int scancode, int action, int mods)
	{
		// W S
		if (key == GLFW_KEY_W)
			this->camera.W = action != GLFW_RELEASE; // PRESS and HOLD = true
		if (key == GLFW_KEY_S)
			this->camera.S = action != GLFW_RELEASE; // RELEASE = false

		// A D
		if (key == GLFW_KEY_A)
			this->camera.A = action != GLFW_RELEASE;
		if (key == GLFW_KEY_D)
			this->camera.D = action != GLFW_RELEASE;

		// Q E
		if (key == GLFW_KEY_Q)
			this->camera.Q = action != GLFW_RELEASE;
		if (key == GLFW_KEY_E)
			this->camera.E = action != GLFW_RELEASE;


		// Ctrl to toggle mouse lock
		if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS)
		{
			this->window->ToggleCursor(this->mouseLock);
			this->mouseLock = !this->mouseLock;
		}

		// Sprint with shift
		if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_RELEASE))
		{
			this->camera.modifier = (action == GLFW_PRESS) ? 2.0f : 1.0f;
		}


		// R to reload compute shader
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			delete this->computeShader;
			this->computeShader = new ComputeShader();

			this->computeShader->InitShader("../resources/compute/rayTracer.glsl");
		}


		// Esc or Space to exit the program
		if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_SPACE)
		{
			fprintf(stderr, "--- End of ray tracer ---\n");
			this->window->Close();
		}
	});

	window->SetMousePressFunction([this](int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
			lClicking = true;
		else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
			lClicking = false;

		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			rClicking = true;
		else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			rClicking = false;
	});

	window->SetMouseScrollFunction([this](double x, double y)
	{
		this->camera.speed += y*0.5;
		if (this->camera.speed < 0.0f)
			this->camera.speed = 0.0f;
	});

	window->SetMouseMoveFunction([this](float x, float y)
	{
		// Only update rotation if mouse is locked
		if (this->mouseLock)
		{
			this->camera.hAngle -= (x - this->window->GetWidth() * 0.5f) * mouseSpeed;
			this->camera.vAngle -= (y - this->window->GetHeight() * 0.5f) * mouseSpeed;


			if (this->camera.vAngle < -80.0f)
				this->camera.vAngle = -80.0f;
			else if(this->camera.vAngle > 80.0f)
				this->camera.vAngle = 80.0f;
		}

		if (lClicking)
		{}
		if (rClicking)
		{}

		oldMouseX = x;
		oldMouseY = y;
	});



	if (this->window->Open())
	{
		this->window->SetTitle("OpenGL Ray Tracer");


		// Turn off V-Sync (I already block while rendering each image, so no screen tearing will occur)
		glfwSwapInterval(0);

		// Set clear color to gray (Background)
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


		// The quad that the ray traced scene will be rendered to
		this->quad = new FullScreenQuad();

		// Define the texture that the compute shader will write to
		this->texWidth = this->window->GetWidth();
		this->texHeight = this->window->GetHeight();
		glGenTextures(1, &this->frameBuffer);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->frameBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->texWidth, this->texHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, this->frameBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


		// Compile the compute shader program
		this->computeShader = new ComputeShader();
		this->computeShader->InitShader("../resources/compute/rayTracer.glsl");


		// Load all the objects for the scene
		this->CreateObjects();
	
		

		// Generate the buffers
		glGenBuffers(1, &this->staticSSBO);
		glGenBuffers(1, &this->dynamicSSBO);

		this->SendBuffer(this->staticGO, staticSSBO, true);
		this->SendBuffer(this->dynamicGO, dynamicSSBO, false);


		// Bind UI render function
		this->window->SetUiRender([this]()
		{
			this->RenderUI();
		});

		return true;
	}
	return false;
}




//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::Run()
{
	Matrix id;

	while (this->window->IsOpen())
	{
		// Set frame start-time
		start = std::chrono::system_clock::now();

		// Get inputs
		this->window->Update();


		// Reset the canvas to remove last frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.8f, 0.0f, 0.8f, 1.0f);


		// Update the view matrix based on input
		this->camera.Move(dt);
		//this->camera.Orbit(Vec3(0,1,0), Vec3(0,1,0), 20.0f * dt);


		CollisionInfo info; //defined in EPA.h
		std::vector<GameObject*> possibleCollisions;
		LineSweep::FindOverlapingAABB(this->staticGO, this->camera, possibleCollisions);
		LineSweep::FindOverlapingAABB(this->dynamicGO, this->camera, possibleCollisions);

		for (int i = 0; i < possibleCollisions.size(); i++)
		{
			Camera *A = &this->camera;
			GameObject *B = possibleCollisions[i];
			bool collision = GJK::GJKIntersect(A, B, info);

			if (collision)
			{
				if (B->isPortal > 0.5f)
				{
					// Treat the camera as a singularity
					Matrix rot = B->transform;
					rot.SetPosition(0, 0, 0);
					Vec3 objNormal = rot * Vec3(0, 0, -1);


					// How much to the side and though the portal you are
					Vec3 offset = this->camera.position - B->transform.GetPosition(); 

					// If the collision and obj normal are oposite, tp the camera
					if (Vec3::Dot(info.collisionNormal, objNormal) < 0.0f &&
					    fabs(offset.x) < 1.0f && fabs(offset.y) < 1.0f && fabs(offset.z) < 1.0f)
					{
						
						// Rotate the offset to match the portal exit	
						offset = Matrix(Vec3(0,1,0), B->cameraRotation) * offset;

						// TP to the exit and add the offset
						this->camera.position = B->portalPosition + offset;	

						// How much to rotate the camera
						this->camera.hAngle += B->cameraRotation;
					}
				}
				else
				{
					// Don't go into walls
					this->camera.position += info.collisionNormal * info.penetrationDepth;
				}

				
				// Same as Move but doesn't use any user input
				this->camera.Update(dt);
			}
		}



		// Rotate objects
		this->dynamicGO[0]->Rotate(Matrix(Vec3(0,1,0), 20.0f * dt)); // Icosphere
		this->dynamicGO[1]->Rotate(Matrix(Vec3(0,0,1), -20.0f * dt)); // Hexagon

		// Center marker
		this->dynamicGO[2]->Rotate(Matrix(Vec3(0,1,0), -20.0f * dt));

		// Planet
		Vec3 planetPos = this->dynamicGO[3]->transform.GetPosition();
		this->dynamicGO[3]->Orbit(Vec3(0, 7, 0), Vec3(0, 1, 0), 50.0f * dt);
		this->dynamicGO[3]->Rotate(Matrix(Vec3(0,1,0), 100.0f * dt));
		Vec3 newPlanetPos = this->dynamicGO[3]->transform.GetPosition();
		Vec3 planetDeltaPos = newPlanetPos - planetPos;

		// Moon
		Matrix tmp = this->dynamicGO[4]->transform;
		tmp.Translate(planetDeltaPos);
		this->dynamicGO[4]->SetTransform(tmp);
		this->dynamicGO[4]->Orbit(newPlanetPos, Vec3(0, 1, 0), -120.0f * dt);
		this->dynamicGO[4]->Rotate(Matrix(Vec3(0,1,0), -100.0f * dt));
		



		// Send the new data to GPU
		this->SendBuffer(this->dynamicGO, this->dynamicSSBO, false);




		// Get the four corner rays of the view frustrum
		Matrix invMVP = Matrix::GetInverse(this->camera.projection * this->camera.view);
		
		Vec4 ray00 = invMVP * Vec4(-1.25f, -1, 0, 1);
		ray00 = ray00 / ray00.w;
		ray00 -= Vec4(this->camera.position, 1.0f);

		Vec4 ray10 = invMVP * Vec4(1.25f, -1, 0, 1);
		ray10 = ray10 / ray10.w;
		ray10 -= Vec4(this->camera.position, 1.0f);

		Vec4 ray01 = invMVP * Vec4(-1.25f, 1, 0, 1);
		ray01 = ray01 / ray01.w;
		ray01 -= Vec4(this->camera.position, 1.0f);

		Vec4 ray11 = invMVP * Vec4(1.25f, 1, 0, 1);
		ray11 = ray11 / ray11.w;
		ray11 -= Vec4(this->camera.position, 1.0f);

		// Send uniforms to shader
		this->computeShader->UseProgram();
		this->computeShader->ModifyVector("eye", this->camera.position);
		this->computeShader->ModifyVector("ray00", Vec3(ray00));
		this->computeShader->ModifyVector("ray10", Vec3(ray10));
		this->computeShader->ModifyVector("ray01", Vec3(ray01));
		this->computeShader->ModifyVector("ray11", Vec3(ray11));



		// Trace the scene to generate an image
		// 32 * 32 groups rendering 1/32^2 pixels of the image
		// 8 * 8 groups rendering 1/8^2 pixels each
		this->computeShader->Draw(this->texWidth/8, this->texHeight/8);
	
		


		// Draw a quad with the texture generated by the ray tracer
		this->quad->Draw(this->frameBuffer);
		


		// Center cursor
		if (this->mouseLock)
			this->window->CenterCursor();



		// Show the rendered buffer on the screen
		this->window->SwapBuffers();

		// Unbind the current program
		glUseProgram(0);

		// Set dt
		this->end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedTime = end - start;
		this->dt = elapsedTime.count();
	}

	delete this->computeShader;
	delete this->quad;
}



//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::RenderUI()
{
	if(this->window->IsOpen())
	{
		ImGui::Begin("Drawing");
			ImGui::Text("FPS: %.2f", 1/(this->dt));
			ImGui::Text("dt: %.4f ms", this->dt*1000.0f);
		ImGui::End();

		ImGui::Begin("Camera");
			Vec3 camPos = this->camera.position;
			Vec3 camDir = this->camera.lookAt - camPos;
			ImGui::Text("Position: %.2f, %.2f, %.2f", camPos.x, camPos.y, camPos.z);
			ImGui::Text("Direction: %.2f, %.2f, %.2f", camDir.x, camDir.y, camDir.z);
			ImGui::Text("Speed: %.2f", this->camera.speed);
		ImGui::End();

		ImGui::Begin("Controls");
			ImGui::Text("Move: WASD");
			ImGui::Text("Up:   E");
			ImGui::Text("Down: Q");
			ImGui::Text(" ");
			ImGui::Text("Change Speed:   Mouse Scroll");
			ImGui::Text("Sprint:         Shift");
			ImGui::Text("Toggle Mouse:   Ctrl");
			ImGui::Text("Reload Shader:  R");
			ImGui::Text("Quit:           Esc/Space");
		ImGui::End();
	}
}


//------------------------------------------------------------------------------
/**
*/
void
ExampleApp::CreateObjects()
{
	GameObject *obj;


	// Load the static scene file
	ObjParser::LoadScene("../resources/models/ray_tracer_scene.obj", this->staticGO);


	obj = ObjParser::LoadMesh("../resources/models/tris/icosphere_lowres_small.obj");
	obj->SetTransform(Vec3(0-50, -2, 0-5), 0);
	obj->color = Vec3(0.0f, 0.0f, 1.0f);
	this->dynamicGO.push_back(obj);

	obj = ObjParser::LoadMesh("../resources/models/quads/hexagon.obj");
	obj->SetTransform(Vec3(0-50, -5, 0-5), 0);
	obj->color = Vec3(0.0f, 0.0f, 1.0f);
	this->dynamicGO.push_back(obj);


	// Spinning markers
	{
		obj = ObjParser::LoadMesh("../resources/models/tris/marker.obj");
		Matrix trans;
		trans.SetPosition(0, 7, 0);
		trans.Scale(2);
		obj->SetTransform(trans);
		obj->color = Vec3(0.8f, 0.8f, 0.0f);
		this->dynamicGO.push_back(obj);

		obj = ObjParser::LoadMesh("../resources/models/tris/icosphere_lowres_small.obj");
		obj->SetTransform(Vec3(-2.5f, 7, 0), 0);
		obj->color = Vec3(0.1f, 0.1f, 0.8f);
		this->dynamicGO.push_back(obj);

		obj = ObjParser::LoadMesh("../resources/models/tris/icosphere_lowres_small.obj");
		trans = Matrix();
		trans.SetPosition(-3.5f, 7, 0);
		trans.Scale(0.2f);
		obj->SetTransform(trans);
		obj->color = Vec3(0.3f, 0.3f, 0.3f);
		this->dynamicGO.push_back(obj);
	}


	// Left Arch
	{
		// Front
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-5, 1, 0.499f), 180);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-6-50, -2, 3.999f);
		obj->portalNormal = Vec3(0, 0, -1);
		this->staticGO.push_back(obj);
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-6-50, -2, -4), 180);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-5, 1, -0.50f);
		obj->portalNormal = Vec3(0, 0, -1);
		this->staticGO.push_back(obj);

		// Back
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-5, 1, -0.499f), 0);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-6-50, -2, -3.999f);
		obj->portalNormal = Vec3(0, 0, 1);
		this->staticGO.push_back(obj);
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-6-50, -2, 4), 0);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-5, 1, 0.50f);
		obj->portalNormal = Vec3(0, 0, 1);
		this->staticGO.push_back(obj);
	}


	// Right Arch
	{
		// Left
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(7.001f, 1, 0), 90);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-3.999f-50, -2, 6);
		obj->portalNormal = Vec3(1, 0, 0);
		obj->cameraRotation = 0;
		this->staticGO.push_back(obj);
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-4-50, -2, 6), 270);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(7, 1, 0);
		obj->portalNormal = Vec3(-1, 0, 0);
		obj->cameraRotation = 0;
		this->staticGO.push_back(obj);


		// Mirror at the back of the corridor
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(4-50, -2, 6), 90);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(3.999f-50, -2, 6);
		obj->portalNormal = Vec3(-1, 0, 0);
		obj->cameraRotation = 180;
		this->staticGO.push_back(obj);
	}


	// Back Arch
	{
		// Left
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-1.5f, 1, -6), 90);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(1.501f, 1, -6);
		obj->portalNormal = Vec3(1, 0, 0);
		this->staticGO.push_back(obj);

		// Right
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(1.5f, 1, -6), 270);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-1.501f, 1, -6);
		obj->portalNormal = Vec3(-1, 0, 0);
		this->staticGO.push_back(obj);
	}


	// Front Arch
	{
		// Enter
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(0, 1, 7.001f), 0);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(3-50, -2, -1.999f);
		obj->portalNormal = Vec3(0, 0, 1);
		obj->cameraRotation = 0;
		this->staticGO.push_back(obj);

		// Exit
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(3-50, -2, -2), 180);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(0, 1, 7);
		obj->portalNormal = Vec3(0, 0, -1);
		obj->cameraRotation = 0;
		this->staticGO.push_back(obj);

		// Loop
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-3-50, -2, 0), 180);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(0-50, -2, 3);
		obj->portalNormal = Vec3(-1, 0, 0);
		obj->cameraRotation = 90;
		this->staticGO.push_back(obj);

	}

	
	// 4D Cube
	{
		// Back portal
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(0, 1, -1.0001f), 0);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(0-50, -5, -1.0001f-5);
		obj->portalNormal = Vec3(0, 0, 1);
		this->staticGO.push_back(obj);

		// Front portal
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(0, 1, 1.0001f), 180);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(0-50, -2, 1.0001f-5);
		obj->portalNormal = Vec3(0, 0, -1);
		this->staticGO.push_back(obj);
		
		// Right portal
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(1.0001f, 1, 0), 270);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(1.0001f-50, -11, 0-5);
		obj->portalNormal = Vec3(-1, 0, 0);
		this->staticGO.push_back(obj);

		// Left portal
		obj = ObjParser::LoadMesh("../resources/models/tris/quad.obj");
		obj->SetTransform(Vec3(-1.0001f, 1, 0), 90);
		obj->isPortal = 1.0f;
		obj->portalPosition = Vec3(-1.0001f-50, -8, 0-5);
		obj->portalNormal = Vec3(1, 0, 0);
		this->staticGO.push_back(obj);
	}
	
}


void
ExampleApp::SendBuffer(std::vector<GameObject*> &go, GLuint ssbo, bool isStatic)
{
	int nrObjects = go.size();
	int nrFloats = nrObjects + 1;
	for (int i = 0; i < nrObjects; i++)
	{
		nrFloats += go[i]->NrValues();

		// 			nrVerts,	isPortal, 	pos,	normal, color
		nrFloats += 1 + 		1 + 		3 +		3 + 	3;
	}

	// Create the array that will be sent to GPU
	float *arr = new float[nrFloats];
	int index = 0;

	arr[index++] = nrObjects;

	// First values are nr of floats in each object
	for (int i = 0; i < nrObjects; i++)
	{
		//   nrVerts, isPortal, position, normal, color = 11
		arr[index++] = go[i]->NrValues() + 11;
	}

	// Add all values
	for (int i = 0; i < nrObjects; i++)
	{
		// Quad or tri
		arr[index++] = go[i]->nrVerts;

		// isPortal
		arr[index++] = go[i]->isPortal;

		// Portal position
		arr[index++] = go[i]->portalPosition.x;
		arr[index++] = go[i]->portalPosition.y;
		arr[index++] = go[i]->portalPosition.z;

		// Portal normal
		arr[index++] = go[i]->portalNormal.x;
		arr[index++] = go[i]->portalNormal.y;
		arr[index++] = go[i]->portalNormal.z;

		// Color of object
		arr[index++] = go[i]->color.x;
		arr[index++] = go[i]->color.y;
		arr[index++] = go[i]->color.z;

		// AABB
		arr[index++] = go[i]->values[0];
		arr[index++] = go[i]->values[1];
		arr[index++] = go[i]->values[2];
		arr[index++] = go[i]->values[3];
		arr[index++] = go[i]->values[4];
		arr[index++] = go[i]->values[5];

		for (int j = 6; j < go[i]->NrValues(); j += 3)
		{
			// Apply transformations
			Vec3 vert(go[i]->values[j], go[i]->values[j+1], go[i]->values[j+2]);
			vert = go[i]->transform * vert;

			// Insert transformed vertex positions
			arr[index++] = vert.x;
			arr[index++] = vert.y;
			arr[index++] = vert.z;
		}
	}



	// Bind the active buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

	// Send the static data to binding 3 once
	if (isStatic)
	{
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * nrFloats, arr, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
	}

	// Send the dynamic data to binding 4 every frame
	else
	{
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * nrFloats, arr, GL_DYNAMIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo);
	}


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	delete[] arr;
}


} // namespace Example