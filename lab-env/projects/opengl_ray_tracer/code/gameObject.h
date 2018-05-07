#pragma once


#include <vector>

#include "mathMatrix.h"
#include "mathVec3.h"


class GameObject
{
private:

	void LoadMesh(const char *filename);


public:
	
	Matrix transform;
	Vec3 portalPosition;
	Vec3 portalNormal;
	Vec3 color;
	Vec3 OBB[8];

	float isPortal = 0.0f;
	float nrVerts = 0.0f;
	int cameraRotation = 0;	// 0, 90, 180, 270


	/*
		Vec3 AABBmin;
		Vec3 AABBmax;
		Vec3 verts[];
	*/
	std::vector<float> values;
	

	GameObject();
	~GameObject();

	int NrValues();
	void SetTransform(const Matrix &m);
	void SetTransform(const Vec3 &pos, const float rot);
	void SetAABB(const Vec3 &min, const Vec3 &max);
	void Rotate(const Matrix &m);
	void Orbit(const Vec3 &point, const Vec3 &axis, const float speed);
};