#include "mathVec4.h"
#include "mathVec3.h"


// Construction
// ----------------------------------------------------------------------------
Vec4::Vec4()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
}

Vec4::Vec4(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vec4::Vec4(const Vec3 &v, float w)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}

// Addition
// ------------------------------------------------------------------------
Vec4 Vec4::operator+(const Vec4 &v)const
{
	Vec4 newVector(
		this->x + v.x,
		this->y + v.y,
		this->z + v.z,
		this->w + v.w);

	return newVector;
}

void Vec4::operator+=(const Vec4 &v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	this->w += v.w;
}


// Subtraction
// ------------------------------------------------------------------------
Vec4 Vec4::operator-(const Vec4 &v)const
{
	Vec4 newVector(
		this->x - v.x,
		this->y - v.y,
		this->z - v.z,
		this->w - v.w);

	return newVector;
}

void Vec4::operator-=(const Vec4 &v)
{
	this->x -= v.x;
	this->y -= v.y;
	this->z -= v.z;
	this->w -= v.w;
}

// Invert
Vec4 Vec4::operator-()const
{
	Vec4 newVector(
		-this->x,
		-this->y,
		-this->z,
		-this->w);

	return newVector;
}


// Multiplication
// ------------------------------------------------------------------------
Vec4 Vec4::operator*(const float f)const
{
	Vec4 newVector(
		this->x * f,
		this->y * f,
		this->z * f,
		this->w * f);

	return newVector;
}

void Vec4::operator*=(const float f)
{
	this->x *= f;
	this->y *= f;
	this->z *= f;
	this->w *= f;
}

Vec4 Vec4::QuaternionMult(const Vec4 &q)const
{
	Vec4 quat;
	quat.x =  this->x * q.w + this->y * q.z - this->z * q.y + this->w * q.x;
	quat.y = -this->x * q.z + this->y * q.w + this->z * q.x + this->w * q.y;
	quat.z =  this->x * q.y - this->y * q.x + this->z * q.w + this->w * q.z;
	quat.w = -this->x * q.x - this->y * q.y - this->z * q.z + this->w * q.w;

	return quat;
}


// Division
// ------------------------------------------------------------------------
Vec4 Vec4::operator/(const float f)const
{
	Vec4 newVector(
		this->x / f,
		this->y / f,
		this->z / f,
		this->w / f);

	return newVector;
}

void Vec4::operator/=(const float f)
{
	this->x /= f;
	this->y /= f;
	this->z /= f;
	this->w /= f;
}


// Equality
// ------------------------------------------------------------------------

// Set equal
void Vec4::operator=(const Vec4 &v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = v.w;
}

// Check equality
bool Vec4::operator==(const Vec4 &v)const
{
	return this->x == v.x &&
		   this->y == v.y &&
		   this->z == v.z &&
		   this->w == v.w;
}

bool Vec4::operator!=(const Vec4 &v)const
{
	return this->x != v.x || 
		   this->y != v.y ||
		   this->z != v.z ||
		   this->w != v.w;
}


// Vector Length
// ------------------------------------------------------------------------
float Vec4::Length()const
{
	float length = sqrt(x*x + y*y + z*z + w*w);
	return length;
}

float Vec4::SquareLength()const
{
	float length = x*x + y*y + z*z + w*w;
	return length;
}


// Vector Normal
// ------------------------------------------------------------------------

// Get vector normal
Vec4 Vec4::Normal()const
{
	const float length = this->Length();

	Vec4 newVector(
		this->x / length,
		this->y / length,
		this->z / length,
		this->w / length);

	return newVector;
}

// Normalize vector
void Vec4::Normalize()
{
	const float length = this->Length();

	this->x /= length;
	this->y /= length;
	this->z /= length;
	this->w /= length;
}


// Static Function
// ------------------------------------------------------------------------

// Dot product
float Vec4::Dot(const Vec4 &v, const Vec4 &u)
{
	float dotProduct =
		v.x * u.x +
		v.y * u.y +
		v.z * u.z +
		v.w * u.w;

	return dotProduct;
}

void Vec4::GetArray(const Vec4 &v, float* array)
{
	array[0] = v.x;
	array[1] = v.y;
	array[2] = v.z;
	array[3] = v.w;
}


// Component-wise Operations
// ------------------------------------------------------------------------

// Multiplication
Vec4 Vec4::ComponentMultiplication(const Vec4 &v, const Vec4 &u)
{
	Vec4 newVector(
		v.x * u.x,
		v.y * u.y,
		v.z * u.z,
		v.w * u.w);

	return newVector;
}

// Division
Vec4 Vec4::ComponentDivision(const Vec4 &v, const Vec4 &u)
{
	Vec4 newVector(
		v.x / u.x,
		v.y / u.y,
		v.z / u.z,
		v.w / u.w);

	return newVector;
}