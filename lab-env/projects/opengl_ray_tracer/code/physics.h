#pragma once

#include <algorithm>

#include "mathMatrix.h"
#include "mathVec3.h"
#include "mathVec4.h"
// #include "GraphicsNode.h"

struct State
{
	// Primary
	Vec3 position;
	Vec3 momentum;

	Vec4 orientation;		// Quaternion
	Vec3 angularMomentum;


	// Secondary
	Vec3 velocity;

	Vec4 spin;				// Quaternion
	Vec3 angularVelocity;

	Matrix inertia;
	Matrix inverseInertia;


	// Constat
	float mass;
	float inverseMass;

	Matrix inverseBodyInertia;



	void Reclalculate()
	{
		Matrix rotation(orientation);
		this->inverseInertia = rotation * inverseBodyInertia * Matrix::GetTranspose(rotation);


		// Movement
		velocity = momentum * inverseMass;

		const float maxAngularMomentum = 10.0f;
		float x =  fminf(fmaxf(angularMomentum.x, -maxAngularMomentum), maxAngularMomentum);
		float y =  fminf(fmaxf(angularMomentum.y, -maxAngularMomentum), maxAngularMomentum);
		float z =  fminf(fmaxf(angularMomentum.z, -maxAngularMomentum), maxAngularMomentum);
		
		angularMomentum = Vec3(x, y, z);

		angularVelocity = inverseInertia * angularMomentum;

		// Normalize the orientation "quaternion" to avoid drifting
		orientation.Normalize();
		
		spin = angularVelocity.QuaternionMult(orientation) * 0.5f;
	}
};

struct Derivative
{
	// Movements
	Vec3 velocity;
	Vec3 force;

	// Rotation
	Vec4 spin;		// Quaternion
	Vec3 torque;
};


class RigidBody
{
private:

	Matrix transform;
	bool isStatic;

	State state;


public:
	RigidBody()
	{
		this->isStatic = false;

		// Default 1m^3 at 100 KG
		this->state.mass = 100.0f;

		// this->state.mass = 0.015f;
		this->state.inverseMass = 1.0f / this->state.mass;

		// Cube
		this->state.inverseBodyInertia[0]  = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
		this->state.inverseBodyInertia[5]  = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
		this->state.inverseBodyInertia[10] = 1.0f / ((1.0f/12.0f) * this->state.mass * (1.0f + 1.0f));
	}

	~RigidBody()
	{
	}


	// Get information

	Vec3 GetPosition()
	{
		return this->state.position;
	}
	Vec3 GetMomentum()
	{
		return this->state.momentum;
	}
	Vec3 GetVelocity()
	{
		return this->state.velocity;
	}
	Vec3 GetAngularMomentum()
	{
		return this->state.angularMomentum;
	}
	Vec3 GetAngularVelocity()
	{
		return this->state.angularVelocity;
	}
	float GetMass()
	{
		if (this->isStatic)
			return 0.0f;
		return this->state.mass;
	}
	float GetInverseMass()
	{
		if (this->isStatic)
			return 0.0f;
		return this->state.inverseMass;
	}
	bool IsStatic()
	{
		return this->isStatic;
	}
	Matrix GetInverseInertia()
	{
		return this->state.inverseInertia;
	}
	Matrix GetTransform()
	{
		return this->transform;
	}



	// Instantaneous teleport to the target position
	void SetPosition(const Vec3 &v)
	{
		this->state.position = v;
		transform[3] = v.x;
		transform[7] = v.y;
		transform[11]= v.z;
	}

	// Instantly adds force to the objecs momentum
	void AddForce(Vec3 f)
	{
		this->state.momentum += f;

		this->state.Reclalculate();
	}
	void AddAngularMomentum(Vec3 f)
	{
		this->state.angularMomentum += f;

		this->state.Reclalculate();
	}

	// Adds rotation around the objects center of mass
	void AddTorque(Vec3 point, Vec3 force)
	{	
		// Get point relative to objects center of mass
		Vec3 localPos = point - this->state.position;

		// Get perpendicular axis to rotate about
		Vec3 rotationAxis = Vec3::Cross(localPos, force);
		this->state.angularMomentum += rotationAxis;

		this->state.Reclalculate();
	}

	// Apply a force to the center of mass (no rotation)
	void ApplyImpulse(Vec3 dir, float f)
	{
		// Make sure direction is normalized and then add force
		dir = dir.Normal() * f;

		// Impulse is added to center of mass point, so no torque is applied
		AddForce(dir);
	}

	// Apply a force a some point on the object causing movement and rotation
	void ApplyImpulseAtWorldPoint(Vec3 point, Vec3 dir, float f)
	{
		// Make sure direction is normalized and then add force
		dir = dir.Normal() * f;

		// Add the force for linear movement
		AddForce(dir);

		// Add torque at a specific point for rotation
		AddTorque(point, dir);
	}

	// Set the mass of the object and recalculate the inertia tensor
	void SetMass(float m)
	{
		this->state.mass = m;
		this->state.inverseMass = 1.0f / m;

		// Ellipsoid  1.0f * 1.0f  0.25f * 0.25f
		// a
		// this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));
		// this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		// this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		// b
		this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (0.25f * 0.25f + 1.0f * 1.0f));
		this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));
		this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.25f * 0.25f));
		// c
		// this->state.inverseBodyInertia[0]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.5f * 0.5f));
		// this->state.inverseBodyInertia[5]  = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 0.5f * 0.5f));
		// this->state.inverseBodyInertia[10] = 1.0f / (1.0f/5.0f * this->state.mass * (1.0f * 1.0f + 1.0f * 1.0f));

		this->state.Reclalculate();
	}

	// Set whether the object should move and react or not
	void SetStatic(bool b)
	{
		this->isStatic = b;
	}
	

	// Returns the velocity of the specified point
	Vec3 VelocityAtWorldPoint(const Vec3 &worldPoint)const
	{
		// Point in local space
		Vec3 localPoint = worldPoint - this->state.position;

		// Angular velocity based on where the point is relative to the rotation axis
		Vec3 vAngular = Vec3::Cross(this->state.angularVelocity, localPoint);

		// Total velocity is linear velocity + angular velocity
		Vec3 pointVelocity = this->state.velocity + vAngular;

		return pointVelocity;
	}



	// Here is where movement from forces is calculated
	Vec3 Force(const State &state, float dt)const
	{
		Vec3 gravity(0, -9.82f, 0);

		return gravity;
	}

	// Here is where rotation from torque is calculated
	Vec3 Torque(const State &state, float dt)const
	{
		Vec3 simpleSpin;// = Vec3(1, 0, 0) - state.angularVelocity * 0.1f;

		return simpleSpin;
	}

	// Evaluates each step during the integration
	Derivative Evaluate(State current, float dt, const Derivative &d)
	{
		// Only changes for MidPoint and RK4
		current.position += d.velocity * dt;
		current.momentum += d.force * dt;

		current.orientation += d.spin * dt;
		current.angularMomentum += d.torque * dt;

		current.Reclalculate();


		// Get the added force and return the derivated values
		Derivative output;
		output.velocity = current.velocity;
		output.force = Force(current, dt);

		output.spin = current.spin;
		output.torque = Torque(current, dt);

		return output;
	}



	// Single step integration
	void Euler(State &state, float dt)
	{	
		// Integrate once
		Derivative a = Evaluate(state, dt, Derivative());

		// Explicit Euler
		state.position += a.velocity * dt;
		state.momentum += a.force * dt;

		state.orientation += a.spin * dt;
		state.angularMomentum += a.torque * dt;


		// Change to this for higher accuracy Semi-Implicit Euler
		// state.velocity += a.force * dt;
		// state.position += a.velocity * dt;

		// Recalculate derived state variables
		state.Reclalculate();
	}

	// Two step integration
	void MidPoint(State &state, float dt)
	{
		Derivative a, b;

		// Take the 4 steps
		a = Evaluate(state, 0.0f, Derivative());
		b = Evaluate(state, dt, a);

		state.position += b.velocity * dt;
		state.momentum += b.force * dt;

		state.orientation += a.spin * dt;
		state.angularMomentum += a.torque * dt;

		state.Reclalculate();
	}

	// Four step integration
	void RK4(State &state, float dt)
	{
		Derivative a, b, c, d;

		// Take the 4 steps
		a = Evaluate(state, 0.0f, Derivative());
		b = Evaluate(state, dt*0.5f, a);
		c = Evaluate(state, dt*0.5f, b);
		d = Evaluate(state, dt, c);

		// Apply with weight of the steps
		Vec3 dPosition = 1.0f / 6.0f * (a.velocity + 2.0f * (b.velocity + c.velocity) + d.velocity);
		Vec3 dMomentum = 1.0f / 6.0f * (a.force + 2.0f * (b.force + c.force) + d.force);

		Vec4 dOrientation = 1.0f / 6.0f * (a.spin + 2.0f * (b.spin + c.spin) + d.spin);
		Vec3 dAngularMomentum = 1.0f / 6.0f * (a.torque + 2.0f * (b.torque + c.torque) + d.torque);


		// Update the current state
		state.position += dPosition * dt;
		state.momentum += dMomentum * dt;

		state.orientation += dOrientation * dt;
		state.angularMomentum += dAngularMomentum * dt;

		// Recalculate derived state variables
		state.Reclalculate();
	}



	void Update(double dt)
	{
		if (this->isStatic)
		{
			this->state.momentum = Vec3();
			this->state.velocity = Vec3();
			this->state.angularMomentum = Vec3();
			this->state.angularVelocity = Vec3();
			return;
		}

		// Drag to reduce speed
		this->state.velocity = this->state.velocity * 0.995f;
		this->state.angularVelocity = this->state.angularVelocity * 0.995f;


		// Euler(this->state, dt);
		// MidPoint(this->state, dt);
		RK4(this->state, dt);


		// Set the final transform of the object
		this->transform = Matrix(this->state.orientation);
		this->transform.SetPosition(this->state.position);
	}

};



// class Ray
// {
// public:
// 	Vec3 start;
// 	Vec3 direction;
// 	Vec3 end;

// 	Ray(){}
// 	~Ray(){}

// 	Ray(double mouseX, double mouseY, int screenWidth, int screenHeight, Matrix view, Matrix projection, Vec3 position)
// 	{
// 		// Move (0,0) from top-left to bottom-left
// 		mouseY = screenHeight - mouseY;


// 		// Convert mouse coords to clip space [-1, 1]
// 		float x = ((float)mouseX/(float)screenWidth - 0.5f) * 2.0f;
// 		float y = ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f;


// 		// Clip space, -1 is the near clipping plane
// 		Vec3 start = Vec3(x, y, -1.0f);
// 		Vec3 end = Vec3(x, y, 0.0f);


// 		// Move start and end from clip to world space
// 		Matrix inverseM = (projection * view).invert();
// 		start = inverseM * start;
// 		start = start / start[3];
// 		end = inverseM * end;
// 		end = end / end[3];


// 		// Save the values in this object
// 		this->start = start;
// 		this->end = end;
// 		this->direction = (end - start).normal();
// 	}


// 	// Intersect box
// 	float AABBIntersect(const Vec3 &min, const Vec3 &max, Vec3 &intersectionPoint)
// 	{
// 		Vec3 tMin = (min - this->start) / this->direction;
// 		Vec3 tMax = (max - this->start) / this->direction;

// 		Vec3 t1, t2;

// 		t1[0] = fminf(tMin[0], tMax[0]);
// 		t1[2] = fminf(tMin[1], tMax[1]);
// 		t1[1] = fminf(tMin[2], tMax[2]);

// 		t2[0] = fmaxf(tMin[0], tMax[0]);
// 		t2[2] = fmaxf(tMin[1], tMax[1]);
// 		t2[1] = fmaxf(tMin[2], tMax[2]);


// 		float tNear = fmaxf(fmaxf(t1[0], t1[1]), t1[2]);
// 		float tFar = fminf(fminf(t2[0], t2[1]), t2[2]);


// 		if (tNear > 0.0f && tNear <= tFar)
// 		{
// 			intersectionPoint = this->start + this->direction * tNear;
// 			return tNear;
// 		}

// 		return -1.0f;
// 	}

	
// 	float OBBIntersect(const Vec3 &min, const Vec3 &max, Matrix transform, Vec3 &intersectionPoint)
// 	{
// 		float tNear = 0.0f;
// 		float tFar = 100000.0f;

// 		Vec3 obbPosition(transform[3], transform[7], transform[11]);
// 		Vec3 delta(obbPosition - this->start);

// 		// Test intersection with x axis
// 		{
// 			Vec3 xAxis(transform[0], transform[4], transform[8]);
// 			float e = xAxis.dot3(delta);
// 			float f = this->direction.dot3(xAxis);
			
// 			// If the line intersects the plane
// 			if (fabsf(f) > 0.001f)
// 			{
// 				float t1 = (e + min[0]) / f; // Intersection with the left plane
// 				float t2 = (e + max[0]) / f; // Intersection with the right plane


// 				// Make sure t1 is the closest point
// 				if (t1 > t2)
// 				{
// 					// Swap
// 					float tmp = t1;
// 					t1 = t2;
// 					t2 = tmp;
// 				}

// 				// tMax is the nearest "far" intersection
// 				if (t2 < tFar)
// 					tFar = t2;
// 				// tMin is the farthest "near" intersection
// 				if (t1 > tNear)
// 					tNear = t1;
				

// 				// If far is closer than near there is no collision (object is behind the camera)
// 				if (tFar < tNear)
// 					return -1.0f;
				
// 			}
// 			else // The ray is parallel to the plane, no intersections
// 			{
// 				if ((-e + min[0] > 0.0f) || (-e + max[0] < 0.0f))
// 					return -1.0f;
// 			}
// 		}

// 		// Test intersection with y axis. Same as above
// 		{
// 			Vec3 yAxis(transform[1], transform[5], transform[9]);
// 			float e = yAxis.dot3(delta);
// 			float f = this->direction.dot3(yAxis);

// 			if (fabsf(f) > 0.001f)
// 			{
// 				float t1 = (e + min[1]) / f;
// 				float t2 = (e + max[1]) / f;

// 				if (t1 > t2)
// 				{
// 					float w = t1; t1 = t2; t2 = w;
// 				}

// 				if (t2 < tFar)
// 					tFar = t2;
// 				if (t1 > tNear)
// 					tNear = t1;
				
// 				if (tFar < tNear)
// 					return -1.0f;
// 			}
// 			else
// 			{
// 				if ((-e + min[1] > 0.0f) || (-e + max[1] < 0.0f))
// 					return -1.0f;
// 			}
// 		}

// 		// Test intersection with z axis. Same as above
// 		{
// 			Vec3 zAxis(transform[2], transform[6], transform[10]);
// 			float e = zAxis.dot3(delta);
// 			float f = this->direction.dot3(zAxis);

// 			if (fabsf(f) > 0.001f)
// 			{
// 				float t1 = (e + min[2]) / f;
// 				float t2 = (e + max[2]) / f;

// 				if (t1 > t2)
// 				{
// 					float w = t1; t1 = t2; t2 = w;
// 				}

// 				if (t2 < tFar)
// 					tFar = t2;
// 				if (t1 > tNear)
// 					tNear = t1;
				
// 				if (tFar < tNear)
// 					return -1.0f;
// 			}
// 			else
// 			{
// 				if ((-e + min[2] > 0.0f) || (-e + max[2] < 0.0f))
// 					return -1.0f;
// 			}
// 		}


// 		intersectionPoint = this->start + this->direction * tNear;
// 		return tNear;
// 	}
// };
