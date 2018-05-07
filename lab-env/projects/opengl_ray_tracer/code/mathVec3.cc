#include "mathVec3.h"
#include "mathVec4.h"


// Construction
// ------------------------------------------------------------------------
Vec3::Vec3()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}
	
Vec3::Vec3(const float x, const float y, const float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3::Vec3(const Vec4 &v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}


// Addition
// ------------------------------------------------------------------------
Vec3 Vec3::operator+(const Vec3 &v)const
{
	Vec3 newVector(
		this->x + v.x,
		this->y + v.y,
		this->z + v.z);

	return newVector;
}

void Vec3::operator+=(const Vec3 &v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
}

Vec3 Vec3::operator+(const float f)const
{
	Vec3 newVector(
		this->x + f,
		this->y + f,
		this->z + f);

	return newVector;
}

void Vec3::operator+=(const float f)
{
	this->x += f;
	this->y += f;
	this->z += f;
}


// Subtraction
// ------------------------------------------------------------------------
Vec3 Vec3::operator-(const Vec3 &v)const
{
	Vec3 newVector(
		this->x - v.x,
		this->y - v.y,
		this->z - v.z);

	return newVector;
}

void Vec3::operator-=(const Vec3 &v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
}

Vec3 Vec3::operator-(const float f)const
{
	Vec3 newVector(
		this->x - f,
		this->y - f,
		this->z - f);

	return newVector;
}

void Vec3::operator-=(const float f)
{
	this->x -= f;
	this->y -= f;
	this->z -= f;
}

// Invert
Vec3 Vec3::operator-()const
{
	Vec3 newVector(
		-this->x,
		-this->y,
		-this->z);

	return newVector;
}


// Multiplication
// ------------------------------------------------------------------------
Vec3 Vec3::operator*(const float f)const
{
	Vec3 newVector(
		this->x * f,
		this->y * f,
		this->z * f);

	return newVector;
}

void Vec3::operator*=(const float f)
{
	this->x *= f;
	this->y *= f;
	this->z *= f;
}

Vec4 Vec3::QuaternionMult(const Vec4 &q)const
{
	Vec4 quat;
	quat.x =  this->x * q.w + this->y * q.z - this->z * q.y + 1.0f * q.x;
	quat.y = -this->x * q.z + this->y * q.w + this->z * q.x + 1.0f * q.y;
	quat.z =  this->x * q.y - this->y * q.x + this->z * q.w + 1.0f * q.z;
	quat.w = -this->x * q.x - this->y * q.y - this->z * q.z + 1.0f * q.w;

	return quat;
}


// Division
// ------------------------------------------------------------------------
Vec3 Vec3::operator/(const float f)const
{
	Vec3 newVector(
		this->x / f,
		this->y / f,
		this->z / f);

	return newVector;
}

void Vec3::operator/=(const float f)
{
	this->x /= f;
	this->y /= f;
	this->z /= f;
}


// Equality
// ------------------------------------------------------------------------

// Set equal
void Vec3::operator=(const Vec3 &v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

// Check equality
bool Vec3::operator==(const Vec3 &v)const
{
	return this->x == v.x && this->y == v.y && this->z == v.z;
}

bool Vec3::operator!=(const Vec3 &v)const
{
	return this->x != v.x || this->y != v.y || this->z != v.z;
}


// Vector Length
// ------------------------------------------------------------------------
float Vec3::Length()const
{
	float length = sqrt(x*x + y*y + z*z);
	return length;
}

float Vec3::SquareLength()const
{
	float length = x*x + y*y + z*z;
	return length;
}


// Vector Normal
// ------------------------------------------------------------------------

// Get vector normal
Vec3 Vec3::Normal()const
{
	const float length = this->Length();

	Vec3 newVector(
		this->x / length,
		this->y / length,
		this->z / length);

	return newVector;
}

// Normalize vector
void Vec3::Normalize()
{
	const float length = this->Length();

	this->x /= length;
	this->y /= length;
	this->z /= length;
}


// Static Function
// ------------------------------------------------------------------------

// Dot product
float Vec3::Dot(const Vec3 &v, const Vec3 &u)
{
	float dotProduct =
		v.x * u.x +
		v.y * u.y +
		v.z * u.z;

	return dotProduct;
}

// Cross product
Vec3 Vec3::Cross(const Vec3 &v, const Vec3 &u)
{
	Vec3 newVector(
		v.y * u.z - v.z * u.y,
		v.z * u.x - v.x * u.z,
		v.x * u.y - v.y * u.x);

	return newVector;
}

void Vec3::GetArray(const Vec3 &v, float* array)
{
	array[0] = v.x;
	array[1] = v.y;
	array[2] = v.z;
}


// Component-wise Operations
// ------------------------------------------------------------------------

// Multiplication
Vec3 Vec3::ComponentMultiplication(const Vec3 &v, const Vec3 &u)
{
	Vec3 newVector(
		v.x * u.x,
		v.y * u.y,
		v.z * u.z);

	return newVector;
}

// Division
Vec3 Vec3::ComponentDivision(const Vec3 &v, const Vec3 &u)
{
	Vec3 newVector(
		v.x / u.x,
		v.y / u.y,
		v.z / u.z);

	return newVector;
}