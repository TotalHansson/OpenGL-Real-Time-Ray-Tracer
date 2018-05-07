#include "gameObject.h"

GameObject::GameObject()
{
	this->color = Vec3(1.0f, 1.0f, 1.0f);
}

GameObject::~GameObject()
{}


int GameObject::NrValues()
{
	return this->values.size();
}

void GameObject::SetTransform(const Matrix &m)
{
	this->transform = m;

	Vec3 min(10000, 10000, 10000);
	Vec3 max = -min;
	Vec3 v;

	// Recalculate AABB
	for (int i = 6; i < values.size(); i+=3)
	{
		v = Vec3(values[i+0], values[i+1], values[i+2]);
		v = m * v;
		// this->values[i+0] = v.x;
		// this->values[i+1] = v.y;
		// this->values[i+2] = v.z;

		// AABB
		if (v.x < min.x)
			min.x = v.x;
		if (v.y < min.y)
			min.y = v.y;
		if (v.z < min.z)
			min.z = v.z;

		if (v.x > max.x)
			max.x = v.x;
		if (v.y > max.y)
			max.y = v.y;
		if (v.z > max.z)
			max.z = v.z;
	}
	this->values[0] = min.x;
	this->values[1] = min.y;
	this->values[2] = min.z;
	this->values[3] = max.x;
	this->values[4] = max.y;
	this->values[5] = max.z;


	// Rotate OBB
	for (int i = 0; i < 8; i++)
	{
		this->OBB[i] = m * this->OBB[i];
	}
}

void GameObject::SetTransform(const Vec3 &pos, const float rot)
{
	Matrix m(Vec3(0,1,0), rot);
	m.SetPosition(pos);
	this->SetTransform(m);
}

void GameObject::SetAABB(const Vec3 &min, const Vec3 &max)
{
	// AABBmax
	this->values.insert(this->values.begin(), max.z);
	this->values.insert(this->values.begin(), max.y);
	this->values.insert(this->values.begin(), max.x);
	// AABBmin
	this->values.insert(this->values.begin(), min.z);
	this->values.insert(this->values.begin(), min.y);
	this->values.insert(this->values.begin(), min.x);
}


void GameObject::Rotate(const Matrix &m)
{
	// Get current transform
	Matrix tmpMat = this->transform;

	// Save pos and move to origo
	Vec3 oldPos = tmpMat.GetPosition();
	tmpMat.SetPosition(0,0,0);

	// Rotate
	tmpMat = m * tmpMat;

	// Move back to pos
	tmpMat.SetPosition(oldPos);

	// Save the new transform
	this->SetTransform(tmpMat);
}

void GameObject::Orbit(const Vec3 &point, const Vec3 &axis, const float speed)
{
	// Get current transform
	Matrix tmpMat = this->transform;
	Vec3 oldPos = tmpMat.GetPosition();

	// obj-obj distance --> obj-origo distance
	Vec3 orbitPos = oldPos - point;
	tmpMat.SetPosition(orbitPos);

	// Rotate
	tmpMat = Matrix(axis, speed) * tmpMat;

	// Move back to pos
	tmpMat.Translate(point);

	// Save the new transform
	this->SetTransform(tmpMat);
}