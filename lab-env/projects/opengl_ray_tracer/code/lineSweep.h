#pragma once

#include <vector>

#include "mathMatrix.h"
#include "mathVec3.h"
#include "gameObject.h"
#include "GJK.h"
#include "camera.h"

class LineSweep
{
public:
	LineSweep()
	{}
	~LineSweep()
	{}

	static void FindOverlapingAABB(const std::vector<GameObject*> &gameObjects,
		const Camera &cam, std::vector<GameObject*> &possibleCollisions)
	{
		Vec3 camMin = cam.position - cam.radius;
		Vec3 camMax = cam.position + cam.radius;

		// Check for collision between each object and the camera
		for (unsigned int i = 0; i < gameObjects.size(); i++)
		{
			Vec3 aabbMin = Vec3(gameObjects[i]->values[0],
								gameObjects[i]->values[1],
								gameObjects[i]->values[2]);

			Vec3 aabbMax = Vec3(gameObjects[i]->values[3],
								gameObjects[i]->values[4],
								gameObjects[i]->values[5]);

				
			if (camMin.x > aabbMax.x ||
				camMax.x < aabbMin.x)
				continue;

			if (camMin.y > aabbMax.y ||
				camMax.y < aabbMin.y)
				continue;

			if (camMin.z > aabbMax.z ||
				camMax.z < aabbMin.z)
				continue;

			// The AABBs are overlaping, possible collision
			possibleCollisions.push_back(gameObjects[i]);
		}
	}
	
};