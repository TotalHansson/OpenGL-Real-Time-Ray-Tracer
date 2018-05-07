#pragma once

#include <cmath>

class Vec3;


class Vec4
{
public:

	float x;
	float y;
	float z;
	float w;


	// Construction
	// ------------------------------------------------------------------------
	Vec4();
	Vec4(float x, float y, float z, float w);
	Vec4(const Vec3 &v, float w);


	// Addition
	// ------------------------------------------------------------------------
	Vec4 operator+(const Vec4 &v)const;
	void operator+=(const Vec4 &v);


	// Subtraction
	// ------------------------------------------------------------------------
	Vec4 operator-(const Vec4 &v)const;
	void operator-=(const Vec4 &v);
	Vec4 operator-()const;


	// Multiplication
	// ------------------------------------------------------------------------
	Vec4 operator*(const float f)const;
	void operator*=(const float f);
	Vec4 QuaternionMult(const Vec4 &q)const;


	// Division
	// ------------------------------------------------------------------------
	Vec4 operator/(const float f)const;
	void operator/=(const float f);
	

	// Equality
	// ------------------------------------------------------------------------
	void operator=(const Vec4 &v);
	bool operator==(const Vec4 &v)const;
	bool operator!=(const Vec4 &v)const;


	// Vector Length
	// ------------------------------------------------------------------------
	float Length()const;
	float SquareLength()const;


	// Vector Normal
	// ------------------------------------------------------------------------
	Vec4 Normal()const;
	void Normalize();


	// Static Function
	// ------------------------------------------------------------------------
	static float Dot(const Vec4 &v, const Vec4 &u);
	static void GetArray(const Vec4 &v, float *array);


	// Component-wise Operations
	// ------------------------------------------------------------------------
	static Vec4 ComponentMultiplication(const Vec4 &v, const Vec4 &u);
	static Vec4 ComponentDivision(const Vec4 &v, const Vec4 &u);



	// Friends
	// ------------------------------------------------------------------------
	friend Vec4 operator+(const float f, const Vec4 &v)
	{
		Vec4 newVector(v.x + f, v.y + f, v.z + f, v.w + f);
		return newVector;
	}
	friend Vec4 operator-(const float f, const Vec4 &v)
	{
		Vec4 newVector(v.x - f, v.y - f, v.z - f, v.w - f);
		return newVector;
	}
	friend Vec4 operator*(const float f, const Vec4 &v)
	{
		Vec4 newVector(v.x * f, v.y * f, v.z * f, v.w * f);
		return newVector;
	}
};