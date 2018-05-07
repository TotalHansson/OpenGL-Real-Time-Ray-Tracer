#include "camera.h"

Camera::Camera()
{
	//this->position = Vec3(0.0f, 7.0f, 17.0f);
	this->position = Vec3(0.0f, 1.5f, 3.5f);

	this->vAngle = -20.0f;
	// this->hAngle = 180.0f;

	this->projection = Matrix(60.0f, 1.0f, 100.0f);
	this->Update(0.0f);
}
Camera::~Camera()
{

}

void Camera::Move(const float dt)
{
	Vec3 camPos = Vec3(0.0f, 0.0f, -1.0f);
	Vec3 camRight = Vec3(1.0f, 0.0f, 0.0f);
	Vec3 camUp = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 camForward = Vec3(0.0f, 0.0f, -1.0f);

	// Camera rotation matrices
	Matrix yaw(camUp, this->hAngle);
	Matrix pitch(camRight, this->vAngle);
	Matrix camRotation = yaw * pitch;


	// Where the camera is looking
	this->lookAt = camRotation * camPos;
	this->lookAt += this->position;

	// Forward vector
	camForward = this->lookAt - this->position;
	camForward.Normalize();

	// Right vector
	camRight = Vec3::Cross(camForward, camUp);
	camRight.Normalize();

	// Up vector
	camUp = Vec3::Cross(camRight, camForward);
	camUp.Normalize();


	if (this->W)
	{
		this->position += camForward * this->speed * this->modifier * dt;
		this->lookAt += camForward * this->speed * this->modifier * dt;
	}
	if (this->S)
	{
		this->position -= camForward * this->speed * this->modifier * dt;
		this->lookAt -= camForward * this->speed * this->modifier * dt;
	}
	if (this->A)
	{
		this->position -= camRight * this->speed * this->modifier * dt;
		this->lookAt -= camRight * this->speed * this->modifier * dt;
	}
	if (this->D)
	{
		this->position += camRight * this->speed * this->modifier * dt;
		this->lookAt += camRight * this->speed * this->modifier * dt;
	}
	if (this->Q)
	{
		this->position -= camUp * this->speed * this->modifier * dt;
		this->lookAt -= camUp * this->speed * this->modifier * dt;
	}
	if (this->E)
	{
		this->position += camUp * this->speed * this->modifier * dt;
		this->lookAt += camUp * this->speed * this->modifier * dt;
	}


	this->view = Matrix(
		this->position,	// Camera position
		this->lookAt,		// Looking at
		camUp					// Up
	);
}

void Camera::Update(const float dt)
{
	Vec3 camPos = Vec3(0.0f, 0.0f, -1.0f);
	Vec3 camRight = Vec3(1.0f, 0.0f, 0.0f);
	Vec3 camUp = Vec3(0.0f, 1.0f, 0.0f);
	Vec3 camForward = Vec3(0.0f, 0.0f, -1.0f);

	// Camera rotation matrices
	Matrix yaw(camUp, this->hAngle);
	Matrix pitch(camRight, this->vAngle);
	Matrix camRotation = yaw * pitch;


	// Where the camera is looking
	this->lookAt = camRotation * camPos;
	this->lookAt += this->position;

	// Forward vector
	camForward = this->lookAt - this->position;
	camForward.Normalize();

	// Right vector
	camRight = Vec3::Cross(camForward, camUp);
	camRight.Normalize();

	// Up vector
	camUp = Vec3::Cross(camRight, camForward);
	camUp.Normalize();
	
	this->view = Matrix(
		this->position,	// Camera position
		this->lookAt,		// Looking at
		camUp					// Up
	);
}

void Camera::Orbit(const Vec3 &point, const Vec3 &axis, const float speed)
{
	this->lookAt = Vec3(0,1,0);

	this->position = Matrix(Vec3(0, 1, 0), -speed*2.0f) * this->position;
	

	// Camera default values
	//Vec3 camPos 	= Vec3(0.0f, 0.4f, 1.0f);
	//Vec3 camRight 	= Vec3(1.0f, 0.0f, 0.0f);
	Vec3 camUp 		= Vec3(0.0f, 1.0f, 0.0f);
	//this->lookAt.y = 1;
	//
	//// Camera rotation matrices
	//Matrix yaw(camUp, this->hAngle);
	//Matrix pitch(camRight, this->vAngle);
	//Matrix camRotation(yaw * pitch);

	//// Set the position of the camera
	//this->position = camRotation * camPos;
	//this->position = this->position.Normal() * 4.0f;

	//// Calculate the right vector for the camera
	//camRight = Vec3::Cross(camUp, this->position.Normal());
	//camRight.Normalize();

	//// Calculate the up vector for the camera
	//camUp = Vec3::Cross(this->position, camRight);
	//camUp.Normalize();


	// Create the view matrix
	this->view =  Matrix(
	 					this->position,	// Camera position
						this->lookAt,		// Looking at
						camUp					// Up
					);

	//this->hAngle += orbitSpeed*dt;
}