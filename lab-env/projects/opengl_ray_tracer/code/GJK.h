#pragma once

#include <vector>

#include "mathMatrix.h"
#include "mathVec3.h"
#include "gameObject.h"
#include "camera.h"
#include "EPA.h"


class GJK
{
public:
	GJK()
	{

	}
	~GJK()
	{}

	// Get the furthest point in a given direction
	static Vec3 MaxPointAlongDirection(const Vec3 direction, GameObject *go)
	{
		// Get OBB
		Vec3 *obb = new Vec3[8];

		// Back half
		obb[0] = Vec3(go->values[0], go->values[1], go->values[2]);
		obb[1] = Vec3(go->values[3], go->values[1], go->values[2]);
		obb[2] = Vec3(go->values[3], go->values[4], go->values[2]);
		obb[3] = Vec3(go->values[0], go->values[4], go->values[2]);

		// Front half
		obb[4] = Vec3(go->values[0], go->values[1], go->values[5]);
		obb[5] = Vec3(go->values[3], go->values[1], go->values[5]);
		obb[6] = Vec3(go->values[3], go->values[4], go->values[5]);
		obb[7] = Vec3(go->values[0], go->values[4], go->values[5]);


		// Get the point with the largest dot product
		float max = -INFINITY;
		Vec3 pos;
		for (unsigned int i = 0; i < 8; i++)
		{
			Vec3 tmpPos = go->transform * obb[i];
			float dot = Vec3::Dot(tmpPos, direction);

			if (dot > max)
			{
				max = dot;
				pos = tmpPos;
			}
		}

		delete[] obb;
		return pos;
	}


	static bool TestOneFace(std::vector<Point> &list, Vec3 &direction,
		Vec3 &ab, Vec3 &ac, Vec3 &ao, Vec3 &abc)
	{
		if (Vec3::Dot(Vec3::Cross(abc, ac), ao) >= 0)
		{
			// Keep A,C
			list[1] = list[2];
			list.pop_back();
			list.pop_back();

			direction = Vec3::Cross(Vec3::Cross(ac, ao), ac);

			return false;
		}


		return TestOneFacePart2(list, direction, ab, ac, ao, abc);
	}

	static bool TestOneFacePart2(std::vector<Point> &list, Vec3 &direction,
		Vec3 &ab, Vec3 &ac, Vec3 &ao, Vec3 &abc)
	{
		if (Vec3::Dot(Vec3::Cross(ab, abc), ao) >= 0)
		{
			// Keep A,B
			list.pop_back();
			list.pop_back();

			direction = Vec3::Cross(Vec3::Cross(ab, ao), ab);

			return false;
		}

		// Keep A,B,C
		list.pop_back();

		direction = abc;

		return false;
	}

	static bool TestTwoFaces(std::vector<Point> &list, Vec3 &direction,
		Vec3 &ab, Vec3 &ac, Vec3 &ad, Vec3 &ao, Vec3 &abc, Vec3 &acd)
	{
		if (Vec3::Dot(Vec3::Cross(abc, ac), ao) >= 0)
		{
			list[1] = list[2];
			list[2] = list[3];

			ab = ac;
			ac = ad;

			abc = acd;

			return TestOneFace(list, direction, ab, ac, ao, abc);
		}

		return TestOneFacePart2(list, direction, ab, ac, ao, abc);
	}

	// If the simplex contains a line
	static void DoLine(std::vector<Point> &list, Vec3 &direction)
	{
		Vec3 ao = -list[0].v;
		Vec3 ab = list[1].v - list[0].v;

		// If the line(ab) is the closest
		if (Vec3::Dot(ab, ao) > 0)
		{
			// List stays the same, contains the line

			// New direction form line to Origo
			direction = Vec3::Cross(Vec3::Cross(ab, ao), ab);
		}

		// Point A is the closest
		else
		{
			// List now only contains A
			list.pop_back();

			// New direction is towards Origo
			direction = ao;
		}
	}

	// If the simplex contains a triangle
	static void DoTriangle(std::vector<Point> &list, Vec3 &direction)
	{
		Vec3 ao = -list[0].v;
		Vec3 ab = list[1].v - list[0].v;
		Vec3 ac = list[2].v - list[0].v;
		Vec3 abc = Vec3::Cross(ab, ac);
		Vec3 abn = Vec3::Cross(ab, abc);
		Vec3 acn = Vec3::Cross(abc, ac);

		
		if (Vec3::Dot(acn, ao) > 0)
		{
			if (Vec3::Dot(ac, ao) > 0)
			{
				// List contains A,C
				list[1] = list[2];
				list.pop_back();

				direction = Vec3::Cross(Vec3::Cross(ac, ao), ac);
			}
			else
			{
				if (Vec3::Dot(ab, ao) > 0)
				{
					// List contains A,B
					list.pop_back();

					direction = Vec3::Cross(Vec3::Cross(ab, ao), ab);
				}
				else
				{
					// List contains A
					list.pop_back();
					list.pop_back();

					direction = ao;
				}
			}
		}

		else
		{
			if (Vec3::Dot(abn, ao) > 0)
			{
				if (Vec3::Dot(ab, ao) > 0)
				{
					// List contains A,B
					list.pop_back();

					direction = Vec3::Cross(Vec3::Cross(ab, ao), ab);
				}
				else
				{
					// List contains A
					list.pop_back();
					list.pop_back();

					direction = ao;
				}
			}
			else
			{
				if (Vec3::Dot(abc, ao) > 0)
				{
					// List keeps A,B,C

					direction = abc;
				}
				else
				{
					// List swaps to A,C,B
					Point tmp = list[1];
					list[1] = list[2];
					list[2] = tmp;

					direction = -abc;
				}
			}
		}
	}

	// If the simple contains a tetrahedron
	static bool DoTetrahedron(std::vector<Point> &list, Vec3 &direction)
	{
		// fprintf(stderr, "DoTetrahedron---\n");

		Vec3 ao = -list[0].v;
		Vec3 ab = list[1].v - list[0].v;
		Vec3 ac = list[2].v - list[0].v;
		Vec3 ad = list[3].v - list[0].v;

		Vec3 abc = Vec3::Cross(ab, ac);
		Vec3 acd = Vec3::Cross(ac, ad);
		Vec3 adb = Vec3::Cross(ad, ab);

		Point tmp;

		const int overAbc = 0x1;
		const int overAcd = 0x2;
		const int overAdb = 0x4;

		/*
			Bit test.
			If Origo is "above" 0 planes 	-> Inside!
			If Origo is "above" 1 plane 	-> Do the same check as the triangle
			If Origo is "above" 2 planes	-> Check one, if it's there run the triangle case, otherwise run it on the other plane
		*/
		int planeTest =
			(Vec3::Dot(abc, ao) > 0.0001f ? overAbc : 0) |
			(Vec3::Dot(acd, ao) > 0.0001f ? overAcd : 0) |
			(Vec3::Dot(adb, ao) > 0.0001f ? overAdb : 0);


		switch(planeTest)
		{	
			// Behind all 3 planes -> Inside the tetrahedron = collicion
			case 0:
				return true;


			case overAbc:
				return TestOneFace(list, direction, ab, ac, ao, abc);


			case overAcd:
				list[1] = list[2];	// b = c
				list[2] = list[3];	// c = d

				return TestOneFace(list, direction, ac, ad, ao, acd);


			case overAdb:
				list[2] = list[1];	// c = b
				list[1] = list[3];	// b = d

				return TestOneFace(list, direction, ad, ab, ao, adb);
				

			case overAbc | overAcd:
				return TestTwoFaces(list, direction, ab, ac, ad, ao, abc, acd);
				

			case overAcd | overAdb:
				tmp = list[1];
				list[1] = list[2];	// b = c
				list[2] = list[3];	// c = d
				list[3] = tmp;		// d = b

				return TestTwoFaces(list, direction, ac, ad, ab, ao, acd, adb);
				

			case overAdb | overAbc:
				tmp = list[2];
				list[2] = list[1];	// c = b
				list[1] = list[3];	// b = d
				list[3] = tmp;		// d = c

				return TestTwoFaces(list, direction, ad, ab, ac, ao, adb, abc);


			default:
				fprintf(stderr, "[GJK - DoTetrahedron]: Default case. This should hopefully never happen\n");
				return true;

		}
	}


	static bool DoSimplex(std::vector<Point> &list, Vec3 &direction)
	{
		int size = list.size();

		if (size == 2)
			DoLine(list, direction);

		else if(size == 3)
			DoTriangle(list, direction);

		else if(size == 4)
			return DoTetrahedron(list, direction);

		return false;
	}


	// Implementing GJK - 2006
	// Casey Muratori
	// https://www.youtube.com/watch?v=Qupqu1xe7Io

	static bool GJKIntersect(GameObject *A, GameObject *B, CollisionInfo &info)
	{	
		int nrLine = 0;
		int nrTriangle = 0;
		int nrTetrahedron = 0;

		// Start by looking in some direction
		Point start = EPA::Support(Vec3(0.0f, 1.0f, 0.0f), A, B);

		// Create a list and add the start value
		std::vector<Point> list;
		list.push_back(start);

		// Look from the starting point toward (0,0,0)
		Vec3 direction = -start.v;

		int stuck = 0;

		while(true)
		{	
			// Get the point opposite to the start 
			Point newPoint = EPA::Support(direction, A, B);

			// If we did not pass Origo there is no overlap
			if (Vec3::Dot(newPoint.v, direction) < 0)
				return false;

			// Add the newest point as the first element
			list.insert(list.begin(), newPoint);


			if(list.size() == 2)
				nrLine++;
			else if(list.size() == 3)
				nrTriangle++;
			else if(list.size() == 4)
				nrTetrahedron++;

			
			if (DoSimplex(list, direction))
			{
				// fprintf(stderr, "Exiting GJK after %i, %i, %i\n", nrLine, nrTriangle, nrTetrahedron);


				info = EPA::GetCollisionInfo(list, A, B);

				return true;
			}

			fprintf(stderr, "Stuck? %i\n", stuck);
		}
	}
	static bool GJKIntersect(Camera *A, GameObject *B, CollisionInfo &info)
	{	
		int nrLine = 0;
		int nrTriangle = 0;
		int nrTetrahedron = 0;

		// Start by looking in some direction
		Point start = EPA::Support(Vec3(0.0f, 1.0f, 0.0f), A, B);

		// Create a list and add the start value
		std::vector<Point> list;
		list.push_back(start);

		// Look from the starting point toward (0,0,0)
		Vec3 direction = -start.v;

		while(true)
		{	
			// Get the point opposite to the start 
			Point newPoint = EPA::Support(direction, A, B);

			// If we did not pass Origo there is no overlap
			if (Vec3::Dot(newPoint.v, direction) < 0)
				return false;

			// Add the newest point as the first element
			list.insert(list.begin(), newPoint);


			if(list.size() == 2)
				nrLine++;
			else if(list.size() == 3)
				nrTriangle++;
			else if(list.size() == 4)
				nrTetrahedron++;

			
			if (DoSimplex(list, direction))
			{
				// fprintf(stderr, "Exiting GJK after %i, %i, %i\n", nrLine, nrTriangle, nrTetrahedron);


				info = EPA::GetCollisionInfo(list, A, B);

				return true;
			}
		}
	}
	
};