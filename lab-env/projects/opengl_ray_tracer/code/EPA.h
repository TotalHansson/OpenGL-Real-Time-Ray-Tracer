#pragma once

#include <vector>

#include "mathMatrix.h"
#include "mathVec3.h"
#include "gameObject.h"
#include "camera.h"

struct Point
{
	Vec3 v;

	Vec3 supA;
	Vec3 supB;

	Point()
	{}

	void operator=(const Point &p)
	{
		this->v = p.v;
		this->supA = p.supA;
		this->supB = p.supB;
	}
};

struct Edge
{
	float distance;
	Point start;
	Point end;

	Edge(Point a, Point b)
	{
		start = a;
		end = b;
	}
};

struct Triangle
{
	float distance;
	Point points[3];
	Vec3 normal;

	Triangle(Point a, Point b, Point c)
	{
		points[0] = a;
		points[1] = b;
		points[2] = c;

		Vec3 u(b.v - a.v);
		Vec3 v(c.v - a.v);
		normal = Vec3::Cross(u, v);
		normal.Normalize();
	}
};

struct CollisionInfo
{
	Vec3 worldPosition;
	Vec3 collisionNormal;
	float penetrationDepth;
};


class EPA
{
public:
	EPA();
	~EPA();


	// Get the furthest point in a given direction
	static Vec3 MaxPointAlongDirection(const Vec3 direction, GameObject *go)
	{
		unsigned int size = go->values.size();

		// Get the point with the largest dot product
		float max = -INFINITY;
		Vec3 pos;
		
		for (unsigned int i = 6; i < size; i += 3)
		{
			Vec3 tmpPos = go->transform * Vec3(go->values[i], go->values[i+1], go->values[i+2]);
			float dot = Vec3::Dot(tmpPos, direction);

			if (dot > max)
			{
				max = dot;
				pos = tmpPos;
			}
		}

		return pos;
	}
	static Vec3 MaxPointAlongDirection(const Vec3 direction, Camera *cam)
	{
		return cam->position + direction*cam->radius;
	}

	static Point Support(Vec3 direction, GameObject *A, GameObject *B)
	{
		direction.Normalize();

		Point p;
		p.supA = MaxPointAlongDirection( direction, A);
		p.supB = MaxPointAlongDirection(-direction, B);
		p.v = p.supA - p.supB;

		return p;
	}
	static Point Support(Vec3 direction, Camera *A, GameObject *B)
	{
		direction.Normalize();

		Point p;
		p.supA = MaxPointAlongDirection( direction, A);
		p.supB = MaxPointAlongDirection(-direction, B);
		p.v = p.supA - p.supB;

		return p;
	}


	// Returns the face closest to Origo
	static int FindClosestFace(std::vector<Triangle> &faces)
	{
		int closestIndex = 0;
		float minDistance = INFINITY;

		// Loop over all faces and get the index of the face closes to Origo
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			faces[i].distance = Vec3::Dot(faces[i].points[0].v, faces[i].normal);

			if (faces[i].distance < minDistance)
			{
				minDistance = faces[i].distance;
				closestIndex = i;
			}
		}

		return closestIndex;
	}


	static void AddEdge(Point &a, Point &b, std::vector<Edge> &edges)
	{
		// If the edge is already in the list, remove it
		for (std::vector<Edge>::iterator it = edges.begin(); it != edges.end(); ++it)
		{
			if (it->start.v == b.v && it->end.v == a.v)
			{
				edges.erase(it);
				return;
			}
		}

		// Else, add it
		edges.push_back(Edge(a, b));
	}

	static Vec3 Barycentric(const Vec3 &p, const Vec3 &a, const Vec3 &b, const Vec3 &c)
	{
		Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
		float d00 = Vec3::Dot(v0, v0);
		float d01 = Vec3::Dot(v0, v1);
		float d11 = Vec3::Dot(v1, v1);
		float d20 = Vec3::Dot(v2, v0);
		float d21 = Vec3::Dot(v2, v1);
		float denom = d00*d11 - d01*d01;

		float v = (d11*d20 - d01*d21) / denom;
		float w = (d00*d21 - d01*d20) / denom;
		float u = 1.0f - v - w;

		return Vec3(u, v, w);
	}

	static CollisionInfo GetCollisionInfo(std::vector<Point> &list, GameObject *A, GameObject *B)
	{
		// Create the initial polytope from the tetrahedron built by GJK
		std::vector<Triangle> faces;
		faces.push_back(Triangle(list[0], list[1], list[2]));
		faces.push_back(Triangle(list[0], list[2], list[3]));
		faces.push_back(Triangle(list[0], list[3], list[1]));
		faces.push_back(Triangle(list[1], list[3], list[2]));


		// Edges that will be around the hole when faces are removed
		std::vector<Edge> edges;

		while(true)
		{
			// Get the closest face to Origo
			int closest = FindClosestFace(faces);

			// Find a new point in the closest face's normal direction
			Point newPoint = Support(faces[closest].normal, A, B);
			// newPoint[3] = 0.0f;

			float distance = Vec3::Dot(newPoint.v, faces[closest].normal);



			// If the distance to newPoint is the same as the closest face we have the correct face
			if (distance - faces[closest].distance < 0.0001f)
			{
				Vec3 bary = Barycentric(faces[closest].normal * distance,
											faces[closest].points[0].v,
											faces[closest].points[1].v,
											faces[closest].points[2].v);

				Vec3 worldCollisionPoint = (bary.x * faces[closest].points[0].supA +
											bary.y * faces[closest].points[1].supA +
											bary.z * faces[closest].points[2].supA);



				CollisionInfo info;
				info.worldPosition = worldCollisionPoint;
				info.collisionNormal = -faces[closest].normal;
				info.penetrationDepth = distance;

				return info;
			}


	
			// Remove all faces that can be seen from the new point and make
			//  a list of the edges that form the hole
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				// If the current face is visible from the new point
				// if (faces[i].normal.dot3(newPoint) > 0.00001f)
				if (Vec3::Dot(faces[i].normal, (newPoint.v - faces[i].points[0].v)) > 0.00001f)
				{
					// Add all edges to the list
					AddEdge(faces[i].points[0], faces[i].points[1], edges);
					AddEdge(faces[i].points[1], faces[i].points[2], edges);
					AddEdge(faces[i].points[2], faces[i].points[0], edges);

					// Remove the face
					faces.erase(faces.begin()+i);
					i--;
				}
			}


			// Make a new face for each of the edges
			for (unsigned int i = 0; i < edges.size(); i++)
			{
				Triangle t(newPoint, edges[i].start, edges[i].end);

				faces.push_back(t);
			}


			// Remove any and all edges
			edges.clear();
		}
	}
	static CollisionInfo GetCollisionInfo(std::vector<Point> &list, Camera *A, GameObject *B)
	{
		// Create the initial polytope from the tetrahedron built by GJK
		std::vector<Triangle> faces;
		faces.push_back(Triangle(list[0], list[1], list[2]));
		faces.push_back(Triangle(list[0], list[2], list[3]));
		faces.push_back(Triangle(list[0], list[3], list[1]));
		faces.push_back(Triangle(list[1], list[3], list[2]));


		// Edges that will be around the hole when faces are removed
		std::vector<Edge> edges;

		while(true)
		{
			// Get the closest face to Origo
			int closest = FindClosestFace(faces);

			// Find a new point in the closest face's normal direction
			Point newPoint = Support(faces[closest].normal, A, B);
			float distance = Vec3::Dot(newPoint.v, faces[closest].normal);


			/*
				Fryser inte när man har cm precision... tror jag?
				Testa lite mer
			*/


			// If the distance to newPoint is the same as the closest face we have the correct face
			if (distance - faces[closest].distance < 0.001f)
			{
				Vec3 bary = Barycentric(faces[closest].normal * distance,
											faces[closest].points[0].v,
											faces[closest].points[1].v,
											faces[closest].points[2].v);

				Vec3 worldCollisionPoint = (bary.x * faces[closest].points[0].supA +
											bary.y * faces[closest].points[1].supA +
											bary.z * faces[closest].points[2].supA);

				CollisionInfo info;
				info.worldPosition = worldCollisionPoint;
				info.collisionNormal = -faces[closest].normal;
				info.penetrationDepth = distance;

				return info;
			}


	
			// Remove all faces that can be seen from the new point and make
			//  a list of the edges that form the hole
			for (unsigned int i = 0; i < faces.size(); i++)
			{
				// If the current face is visible from the new point
				// if (faces[i].normal.dot3(newPoint) > 0.00001f)
				if (Vec3::Dot(faces[i].normal, (newPoint.v - faces[i].points[0].v)) > 0.00001f)
				{
					// Add all edges to the list
					AddEdge(faces[i].points[0], faces[i].points[1], edges);
					AddEdge(faces[i].points[1], faces[i].points[2], edges);
					AddEdge(faces[i].points[2], faces[i].points[0], edges);

					// Remove the face
					faces.erase(faces.begin()+i);
					i--;
				}
			}


			// Make a new face for each of the edges
			for (unsigned int i = 0; i < edges.size(); i++)
			{
				Triangle t(newPoint, edges[i].start, edges[i].end);

				faces.push_back(t);
			}


			// Remove any and all edges
			edges.clear();
		}
	}
	
};