#version 430 core
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D frameBuffer;


const int MAX_PORTAL_DEPTH = 5;
const float MAX_SCENE_BOUNDS = 1000.0f;
const float PI = 3.1415926f;
const float EPSILON = 0.00001f;
const vec3 BACKGROUND_COLOR = vec3(0.1f, 0.1f, 0.1f);
const int constObjSize = 17;

const mat4 rot90  = mat4(0.0f, 0.0f, 1.0f, 0.0f, 
						 0.0f, 1.0f, 0.0f, 0.0f,
						-1.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

const mat4 rot180 = mat4(-1.0f, 0.0f, 0.0f, 0.0f,
						  0.0f, 1.0f, 0.0f, 0.0f,
						  0.0f, 0.0f,-1.0f, 0.0f,
						  0.0f, 0.0f, 0.0f, 1.0f);

const mat4 rot270 = mat4(0.0f, 0.0f,-1.0f, 0.0f,
						 0.0f, 1.0f, 0.0f, 0.0f,
						 1.0f, 0.0f, 0.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);

const vec3 light = vec3(-0.8f, 2.0f, -0.4f);


layout(std430, binding = 3) buffer StaticVertexBuffer
{
	/*
		1x float nrObjects
		nrObjects float nrValues

		nrObjects *
		{
			1x float nrVerts (3/4)
			1x float isPortal
			3x float portalCornerTL
			3x float portalCornerBR
			3x float RGB
			3x float AABB.min
			3x float AABB.max
			
			nrFloats *
			{
				1x float x
				1x float y
				1x float z
			}
		}
	*/
	float staticBuffer[];
};
layout(std430, binding = 4) buffer DynamicVertexBuffer
{
	float dynamicBuffer[];
};



// Camera specification
uniform vec3 eye;
uniform vec3 ray00;
uniform vec3 ray10;
uniform vec3 ray01;
uniform vec3 ray11;



struct Ray
{
	vec3 origin;
	vec3 dir;
};

struct ObjInfo
{
	int nrVerts;
	bool isPortal;
	vec3 exitPortalPosition;
	vec3 exitPortalNormal;
	vec3 color;
};

struct AABB
{
	vec3 min;
	vec3 max;
};

struct HitInfo
{
	vec3 color;
	vec3 normal;
	float distance;

	bool isPortal;
	vec2 hitCoords;
	int corner;
	vec3 exitPortalPosition;
	vec3 exitPortalNormal;	
};


bool IsSame(const float point, const float value)
{
	if (point < value + EPSILON && point > value - EPSILON)
		return true;
	
	return false;
}


vec2 IntersectAABB(const Ray ray, const AABB aabb)
{
	vec3 oneOverRayDir = 1.0f / ray.dir;
	vec3 tMin = (aabb.min - ray.origin) * oneOverRayDir;
	vec3 tMax = (aabb.max - ray.origin) * oneOverRayDir;
	vec3 t1 = min(tMin, tMax);
	vec3 t2 = max(tMin, tMax);
	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	// If near hit is infront of origin, and near is closer than far
	return vec2(tNear, tFar);
}

// Möller-Trumbore "Fast, Minimum Storage Ray/Triangle Intersection"
// https://dl.acm.org/citation.cfm?id=1198746
bool IntersectTriangle(const Ray ray, const vec3 a, const vec3 ab, const vec3 ac,
					   out float triangleDistance, out vec2 hitCoords)
{
	vec3 pvec = cross(ray.dir, ac);
	float det = dot(ab, pvec);

	// If determinant is close to 0 the ray is parallel to the triangle
	if (det < EPSILON) // single-sided
		return false;

	float invDet = 1.0f / det;

	// u needs to be in range (0,1)
	vec3 tvec = ray.origin - a;
	float u = dot(tvec, pvec) * invDet;
	if (u < 0 || u > 1)
		return false;
	

	// u+v needs to be in range (0,1)
	vec3 qvec = cross(tvec, ab);
	float v = dot(ray.dir, qvec) * invDet;
	if (v < 0 || u + v > 1)
		return false;
	

	// Distance to collision point
	triangleDistance = dot(ac, qvec) * invDet;
	hitCoords = vec2(u, v);

	return true;
}


bool IntersectObject(const Ray ray, const ObjInfo obj, const int thisNrValues, const int objStart, const bool dynamic,
					 out float distance, out HitInfo info)
{
	bool triangleHit = false;
	float closestTriangle = MAX_SCENE_BOUNDS;
	vec3 a,b,c,d;

	int stop = objStart + thisNrValues - constObjSize;

	if (obj.nrVerts == 3)
	{
		// Loop over all triangles in the object
		for (int i = objStart; i < stop; i+=9)
		{
			if (dynamic)
			{
				c = vec3(dynamicBuffer[i+0], dynamicBuffer[i+1], dynamicBuffer[i+2]);
				a = vec3(dynamicBuffer[i+3], dynamicBuffer[i+4], dynamicBuffer[i+5]);
				b = vec3(dynamicBuffer[i+6], dynamicBuffer[i+7], dynamicBuffer[i+8]);
			}
			else
			{
				c = vec3(staticBuffer[i+0], staticBuffer[i+1], staticBuffer[i+2]);
				a = vec3(staticBuffer[i+3], staticBuffer[i+4], staticBuffer[i+5]);
				b = vec3(staticBuffer[i+6], staticBuffer[i+7], staticBuffer[i+8]);
			}

			vec3 ab = b - a;
			vec3 ac = c - a;

			float triangleDistance = MAX_SCENE_BOUNDS;
			vec2 hitCoords;
			bool hitTriangle = IntersectTriangle(ray, a, ab, ac,
										  triangleDistance, hitCoords);

			if (hitTriangle && triangleDistance < closestTriangle && triangleDistance > 0.0f)
			{
				// Hit top-left triangle of portal
				if (i == objStart)
					info.corner = 1;
								
				// Hit bottom-right triangle of portal
				else if (i == objStart+9)
					info.corner = 2;


				if (hitCoords.x < 0.01f || hitCoords.y < 0.01f || hitCoords.x + hitCoords.y > 0.99f)
					info.color = vec3(0.0f, 0.0f, 0.0f);
				else
					info.color = obj.color;

				
				info.normal = cross(ab, ac);
				info.isPortal = obj.isPortal;
				info.exitPortalPosition = obj.exitPortalPosition;
				info.exitPortalNormal = obj.exitPortalNormal;
				info.hitCoords = hitCoords;

				closestTriangle = triangleDistance;
				triangleHit = true;
			}
		}
	}
	else
	{
		// Loop over all quads in the object
		for (int i = objStart; i < stop; i+=12)
		{
			if (dynamic)
			{
				c = vec3(dynamicBuffer[i+0], dynamicBuffer[i+1], dynamicBuffer[i+2]);
				a = vec3(dynamicBuffer[i+3], dynamicBuffer[i+4], dynamicBuffer[i+5]);
				b = vec3(dynamicBuffer[i+6], dynamicBuffer[i+7], dynamicBuffer[i+8]);
				d = vec3(dynamicBuffer[i+9], dynamicBuffer[i+10], dynamicBuffer[i+11]);
			}
			else
			{
				c = vec3(staticBuffer[i+0], staticBuffer[i+1], staticBuffer[i+2]);
				a = vec3(staticBuffer[i+3], staticBuffer[i+4], staticBuffer[i+5]);
				b = vec3(staticBuffer[i+6], staticBuffer[i+7], staticBuffer[i+8]);
				d = vec3(staticBuffer[i+9], staticBuffer[i+10], staticBuffer[i+11]);
			}

			vec3 ab = b - a;
			vec3 ac = c - a;
			vec3 db = b - d;
			vec3 dc = c - d;

			float triangleDistance = MAX_SCENE_BOUNDS;
			vec2 hitCoords;
			bool hitTriangle = IntersectTriangle(ray, a, ab, ac,
													triangleDistance, hitCoords);
			
			if (hitTriangle && triangleDistance < closestTriangle && triangleDistance > 0.0f)
			{
				if (hitCoords.x < 0.01f || hitCoords.y < 0.01f)
					info.color = vec3(0.0f, 0.0f, 0.0f);
				else
					info.color = obj.color;
				
				info.normal = cross(ab, ac);
				info.isPortal = obj.isPortal;
				info.hitCoords = hitCoords;

				closestTriangle = triangleDistance;
				triangleHit = true;
			}

			
			hitTriangle = IntersectTriangle(ray, d, dc, db,
											triangleDistance, hitCoords);
			if (hitTriangle && triangleDistance < closestTriangle && triangleDistance > 0.0f)
			{
				if (hitCoords.x < 0.01f || hitCoords.y < 0.01f)
					info.color = vec3(0.0f, 0.0f, 0.0f);
				else
					info.color = obj.color;
				
				
				info.normal = cross(dc, db);
				info.isPortal = obj.isPortal;
				info.hitCoords = hitCoords;

				closestTriangle = triangleDistance;
				triangleHit = true;
			}
		}
	}



	distance = closestTriangle;
	return triangleHit;
}


bool IntersectScene(const Ray ray,
	out HitInfo info)
{
	ObjInfo obj;
	AABB aabb;

	bool intersectSomething = false;
	float smallestDistance = MAX_SCENE_BOUNDS;
	float distance = MAX_SCENE_BOUNDS;
	float closestAABBFar = MAX_SCENE_BOUNDS;
	vec3 closestColor;
	vec3 closestNormal;
	bool closestIsPortal = false;


	// Calculate background color
	float dirDotY = dot(ray.dir, vec3(0.0f, 1.0f, 0.0f));
	float height = sin(dirDotY);
	if (dirDotY > -0.1f)
		closestColor = vec3(0.0f, 0.5f, 1.0f) * 2 * (height + 0.11f);
	else
		closestColor = vec3(0.3f, 0.4f, 0.3f) * 2 * (-height - 0.08f);
	
	

	int nrStaticObjects = int(staticBuffer[0] + 0.5f);
	int nrDynamicObjects = int(dynamicBuffer[0] + 0.5f);
	int objStart = 1 + nrStaticObjects;

	// Loop over all objects
	for (int i = 0; i < nrStaticObjects + nrDynamicObjects; i++)
	{
		// Nr of floats for this object
		int thisNrValues;

		// Reset objStart when going over to the dynamic buffer
		if (i == nrStaticObjects)
			objStart = 1 + nrDynamicObjects;

		// Get obj information
		if (i < nrStaticObjects)
		{
			thisNrValues	 		=  int(staticBuffer[1+i] + 0.5f);
			obj.nrVerts 			=  int(staticBuffer[objStart+0] + 0.5f);
			obj.isPortal 			=     (staticBuffer[objStart+1] > 0.5f);
			obj.exitPortalPosition 	= vec3(staticBuffer[objStart+2], staticBuffer[objStart+3], staticBuffer[objStart+4]);
			obj.exitPortalNormal 	= vec3(staticBuffer[objStart+5], staticBuffer[objStart+6], staticBuffer[objStart+7]);
			obj.color 				= vec3(staticBuffer[objStart+8], staticBuffer[objStart+9], staticBuffer[objStart+10]);
			aabb.min 				= vec3(staticBuffer[objStart+11], staticBuffer[objStart+12], staticBuffer[objStart+13]);
			aabb.max 				= vec3(staticBuffer[objStart+14], staticBuffer[objStart+15], staticBuffer[objStart+16]);
		}
		else
		{
			thisNrValues 			=  int(dynamicBuffer[1+i-nrStaticObjects] + 0.5f);
			obj.nrVerts 			=  int(dynamicBuffer[objStart+0] + 0.5f);
			obj.isPortal 			= 	  (dynamicBuffer[objStart+1] > 0.5f);
			obj.exitPortalPosition 	= vec3(dynamicBuffer[objStart+2], dynamicBuffer[objStart+3], dynamicBuffer[objStart+4]);
			obj.exitPortalNormal 	= vec3(dynamicBuffer[objStart+5], dynamicBuffer[objStart+6], dynamicBuffer[objStart+7]);
			obj.color 				= vec3(dynamicBuffer[objStart+8], dynamicBuffer[objStart+9], dynamicBuffer[objStart+10]);
			aabb.min 				= vec3(dynamicBuffer[objStart+11], dynamicBuffer[objStart+12], dynamicBuffer[objStart+13]);
			aabb.max 				= vec3(dynamicBuffer[objStart+14], dynamicBuffer[objStart+15], dynamicBuffer[objStart+16]);
		}


		// Check if ray is hitting the AABB
		vec2 aabbHit = IntersectAABB(ray, aabb);
		if (aabbHit.y > 0.0f && aabbHit.x <= aabbHit.y && aabbHit.x < closestAABBFar)
		{
			// Check if the ray is hitting any triangle in the mesh
			bool triangleHit = IntersectObject(ray, obj, thisNrValues, objStart + constObjSize, (i >= nrStaticObjects),
													  distance, info);
			if (triangleHit && distance < smallestDistance)
			{
				// Is the object a portal?
				closestIsPortal = obj.isPortal;
				closestColor = info.color;
				closestNormal = info.normal;
				
				// Values of the closest hit
				smallestDistance = distance;

				closestAABBFar = aabbHit.y;
				intersectSomething = true;
			}
		}

		// Move to the next object
		objStart += thisNrValues;
	}

	info.distance = smallestDistance;
	info.normal = closestNormal;
	info.isPortal = closestIsPortal;
	info.color = closestColor;
	return intersectSomething;
}


bool ShadowIntersectObject(const Ray ray, const int nrVerts, const int thisNrValues, const int objStart, const bool dynamic)
{
	vec3 a,b,c,d;

	int stop = objStart + thisNrValues - constObjSize;

	if (nrVerts == 3)
	{
		// Loop over all triangles in the object
		for (int i = objStart; i < stop; i+=9)
		{
			if (dynamic)
			{
				c = vec3(dynamicBuffer[i+0], dynamicBuffer[i+1], dynamicBuffer[i+2]);
				a = vec3(dynamicBuffer[i+3], dynamicBuffer[i+4], dynamicBuffer[i+5]);
				b = vec3(dynamicBuffer[i+6], dynamicBuffer[i+7], dynamicBuffer[i+8]);
			}
			else
			{
				c = vec3(staticBuffer[i+0], staticBuffer[i+1], staticBuffer[i+2]);
				a = vec3(staticBuffer[i+3], staticBuffer[i+4], staticBuffer[i+5]);
				b = vec3(staticBuffer[i+6], staticBuffer[i+7], staticBuffer[i+8]);
			}

			vec3 ab = b - a;
			vec3 ac = c - a;

			vec2 hitCoords;
			float triangleDistance;
			bool hitTriangle = IntersectTriangle(ray, a, ab, ac,
										  triangleDistance, hitCoords);

			if (hitTriangle && triangleDistance > 0.0f)
				return true;
		}
	}
	else
	{
		// Loop over all quads in the object
		for (int i = objStart; i < stop; i+=12)
		{
			if (dynamic)
			{
				c = vec3(dynamicBuffer[i+0], dynamicBuffer[i+1], dynamicBuffer[i+2]);
				a = vec3(dynamicBuffer[i+3], dynamicBuffer[i+4], dynamicBuffer[i+5]);
				b = vec3(dynamicBuffer[i+6], dynamicBuffer[i+7], dynamicBuffer[i+8]);
				d = vec3(dynamicBuffer[i+9], dynamicBuffer[i+10], dynamicBuffer[i+11]);
			}
			else
			{
				c = vec3(staticBuffer[i+0], staticBuffer[i+1], staticBuffer[i+2]);
				a = vec3(staticBuffer[i+3], staticBuffer[i+4], staticBuffer[i+5]);
				b = vec3(staticBuffer[i+6], staticBuffer[i+7], staticBuffer[i+8]);
				d = vec3(staticBuffer[i+9], staticBuffer[i+10], staticBuffer[i+11]);
			}
			
			vec3 ab = b - a;
			vec3 ac = c - a;
			vec3 db = b - d;
			vec3 dc = c - d;

			float triangleDistance;
			vec2 hitCoords;
			bool hitTriangle = IntersectTriangle(ray, a, ab, ac,
												 triangleDistance, hitCoords);
			
			if (hitTriangle && triangleDistance > 0.0f)
				return true;

			
			hitTriangle = IntersectTriangle(ray, d, dc, db,
											triangleDistance, hitCoords);
			if (hitTriangle && triangleDistance > 0.0f)
				return true;
		}
	}

	return false;
}


bool ShadowIntersectScene(const Ray ray)
{
	ObjInfo obj;
	AABB aabb;

	int nrStaticObjects = int(staticBuffer[0] + 0.5f);
	int nrDynamicObjects = int(dynamicBuffer[0] + 0.5f);
	int nrObjects = nrStaticObjects + nrDynamicObjects;
	int objStart = 1 + nrStaticObjects;

	// Loop over all objects
	for (int i = 0; i < nrObjects; i++)
	{
		// Nr of floats for this object
		int thisNrValues;

		// Reset objStart when going over to the dynamic buffer
		if (i == nrStaticObjects)
			objStart = 1 + nrDynamicObjects;

		// Get obj information
		if (i < nrStaticObjects)
		{
			thisNrValues 	=  int(staticBuffer[1+i] + 0.5f);
			obj.nrVerts 	=  int(staticBuffer[objStart+0] + 0.5f);
			obj.isPortal 	=     (staticBuffer[objStart+1] > 0.5f);
			aabb.min 		= vec3(staticBuffer[objStart+11], staticBuffer[objStart+12], staticBuffer[objStart+13]);
			aabb.max 		= vec3(staticBuffer[objStart+14], staticBuffer[objStart+15], staticBuffer[objStart+16]);
		}
		else
		{
			thisNrValues 	=  int(dynamicBuffer[1+i-nrStaticObjects] + 0.5f);
			obj.nrVerts		=  int(dynamicBuffer[objStart+0] + 0.5f);
			obj.isPortal 	= 	  (dynamicBuffer[objStart+1] > 0.5f);
			aabb.min 		= vec3(dynamicBuffer[objStart+11], dynamicBuffer[objStart+12], dynamicBuffer[objStart+13]);
			aabb.max 		= vec3(dynamicBuffer[objStart+14], dynamicBuffer[objStart+15], dynamicBuffer[objStart+16]);
		}


		// Portals don't block light
		if (!obj.isPortal)
		{
			// Check if ray is hitting the AABB
			vec2 aabbHit = IntersectAABB(ray, aabb);
			if (aabbHit.x <= aabbHit.y)
			{
				// Check if the ray is hitting any triangle in the mesh
				if (ShadowIntersectObject(ray, obj.nrVerts, thisNrValues, objStart + constObjSize, (i >= nrStaticObjects)))
					return true;

			}
		}

		// Move to the next object
		objStart += thisNrValues;
	}

	return false;
}


Ray GenerateNewRay(const HitInfo info, const vec3 rayDir)
{
	Ray r;

	vec3 en = info.normal;
	vec3 n = info.exitPortalNormal;

	// Barycentric coordinates
	float u = info.hitCoords.x;
	float v = info.hitCoords.y;
	float w = 1 - u - v;



	// TR
	vec3 b = vec3(info.exitPortalPosition.x - n.z,
				  info.exitPortalPosition.y + 1.0f,
				  info.exitPortalPosition.z - n.x);
	// BL
	vec3 c = vec3(info.exitPortalPosition.x + n.z,
				  info.exitPortalPosition.y - 1.0f,
				  info.exitPortalPosition.z + n.x);

	

	// If the top-left triangle was hit
	if (info.corner == 1)
	{
		// TL
		vec3 a = vec3(info.exitPortalPosition.x + n.z,
					  info.exitPortalPosition.y + 1.0f,
					  info.exitPortalPosition.z + n.x);
		
		r.origin = u*a + v*c + w*b;
	}

	// The bottom-right triangle was hit
	else if(info.corner == 2)
	{
		// BR
		vec3 d = vec3(info.exitPortalPosition.x - n.z,
					  info.exitPortalPosition.y - 1.0f,
					  info.exitPortalPosition.z - n.x);

		r.origin = u*b + v*c + w*d;
	}
	
	// If normal is along x-axis, flip z coord
	if (n.x < -0.5f || n.x > 0.5f)
	{
		// r.origin.z = -r.origin.z;
		r.origin.z = info.exitPortalPosition.z + (info.exitPortalPosition.z - r.origin.z);
	}
	



	// Normals are opposite, no need to rotate the ray
	if (en.x * n.x < -0.5f || en.z * n.z < -0.5f)
		r.dir = rayDir;
	
	// Normals are rotated 90 degrees (right-hand coordinates)
	else if (en.x * n.z < -0.5f || en.z * n.x >  0.5f)
		r.dir = (rot90 * vec4(rayDir, 1.0f)).xyz;
	
	// Normals are the same, mirror the new ray
	else if (IsSame(en.x, n.x) || IsSame(en.z, n.z))
		r.dir = (rot180 * vec4(rayDir, 1.0f)).xyz;
	
	// Else the normals are rotated 270 degrees
	else
		r.dir = (rot270 * vec4(rayDir, 1.0f)).xyz;


	return r;
}



vec3 Trace(Ray ray, const ivec2 coord)
{
	vec3 color;
	HitInfo info;
	int portalDepth = 0;



	// Intersect the scene
	bool hitSomething = IntersectScene(ray, info);


	// Keep shooting rays until something that isn't a portal is hit
	while (hitSomething && info.isPortal && portalDepth <= MAX_PORTAL_DEPTH)
	{
		// Create a new ray from the portal exit
		ray = GenerateNewRay(info, ray.dir);

		// Intersect the scene with the new ray
		hitSomething = IntersectScene(ray, info);

		// Make sure we don't get stuck in an infinite loop
		portalDepth++;
	}

	// Set the pixel color to color of the object hit
	color = info.color;



	// Basic no-portal shadows
	vec3 hitPoint = ray.origin + ray.dir * info.distance;
	ray.origin = hitPoint + info.normal * EPSILON;
	ray.dir = normalize(light);

	// If the shadow ray intersects something
	if (hitSomething && ShadowIntersectScene(ray))
		color *= 0.4f;


	return color;
}


void main()
{
	ivec2 pixelCoord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 size = imageSize(frameBuffer);

	// Make sure the pixel is inside the bounds of the frame (Do I need this?)
	if (pixelCoord.x >= size.x || pixelCoord.y >= size.y)
		return;
	

	// Create ray
	Ray ray;
	ray.origin = eye;

	vec2 pos = vec2(pixelCoord) / vec2(size.x, size.y);
	ray.dir = mix(mix(ray00, ray01, pos.y), mix(ray10, ray11, pos.y), pos.x);


	// Trace the ray for this pixel
	vec3 color = Trace(ray, pixelCoord);
	

	// Save the output to the texture
	imageStore(frameBuffer, pixelCoord, vec4(color, 1.0f));
}