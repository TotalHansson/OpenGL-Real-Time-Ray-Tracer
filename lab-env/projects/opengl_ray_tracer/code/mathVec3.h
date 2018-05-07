#pragma once

#include <cmath>

class Vec4;


class Vec3
{
public:

	float x;
	float y;
	float z;


	// Construction
	// ------------------------------------------------------------------------
	Vec3();
	Vec3(const float x, const float y, const float z);
	Vec3(const Vec4 &v);


	// Addition
	// ------------------------------------------------------------------------
	Vec3 operator+(const Vec3 &v)const;
	void operator+=(const Vec3 &v);
	Vec3 operator+(const float f)const;
	void operator+=(const float f);


	// Subtraction
	// ------------------------------------------------------------------------
	Vec3 operator-(const Vec3 &v)const;
	void operator-=(const Vec3 &v);
	Vec3 operator-(const float)const;
	void operator-=(const float);
	Vec3 operator-()const;


	// Multiplication
	// ------------------------------------------------------------------------
	Vec3 operator*(const float f)const;
	void operator*=(const float f);
	Vec4 QuaternionMult(const Vec4 &q)const;


	// Division
	// ------------------------------------------------------------------------
	Vec3 operator/(const float f)const;
	void operator/=(const float f);
	

	// Equality
	// ------------------------------------------------------------------------
	void operator=(const Vec3 &v);
	bool operator==(const Vec3 &v)const;
	bool operator!=(const Vec3 &v)const;


	// Vector Length
	// ------------------------------------------------------------------------
	float Length()const;
	float SquareLength()const;


	// Vector Normal
	// ------------------------------------------------------------------------
	Vec3 Normal()const;
	void Normalize();


	// Static Function
	// ------------------------------------------------------------------------
	static float Dot(const Vec3 &v, const Vec3 &u);
	static Vec3 Cross(const Vec3 &v, const Vec3 &u);
	static void GetArray(const Vec3 &v, float *array);


	// Component-wise Operations
	// ------------------------------------------------------------------------
	static Vec3 ComponentMultiplication(const Vec3 &v, const Vec3 &u);
	static Vec3 ComponentDivision(const Vec3 &v, const Vec3 &u);



	// Friends
	// ------------------------------------------------------------------------
	friend Vec3 operator+(const float f, const Vec3 &v)
	{
		Vec3 newVector(v.x + f, v.y + f, v.z + f);
		return newVector;
	}
	friend Vec3 operator-(const float f, const Vec3 &v)
	{
		Vec3 newVector(v.x - f, v.y - f, v.z - f);
		return newVector;
	}
	friend Vec3 operator*(const float f, const Vec3 &v)
	{
		Vec3 newVector(v.x * f, v.y * f, v.z * f);
		return newVector;
	}
};