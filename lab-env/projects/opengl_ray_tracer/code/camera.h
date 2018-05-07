#pragma once

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"

class Camera
{
public:

	Matrix view;
	Matrix projection;
	Vec3 position;
	Vec3 lookAt;
	float radius = 0.25f;
	float speed = 4.0f;
	float modifier = 1.0f;
	float hAngle = 0.0f;
	float vAngle = 0.0f;
	bool W=false, A=false, S=false, D=false, Q=false, E=false;


	Camera();
	~Camera();
	
	void Move(const float dt);
	void Update(const float dt);
	void Orbit(const Vec3 &point, const Vec3 &axis, const float speed);
};