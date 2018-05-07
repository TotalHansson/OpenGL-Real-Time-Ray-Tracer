#pragma once

#include <cmath>

#include "mathVec3.h"
#include "mathVec4.h"

#define PI 3.14159265359f



class Matrix
{
private:

	float m[16] = 
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};


public:

	// Construction
	// ------------------------------------------------------------------------	
	// Identity
	Matrix()
	{

	}

	// Projection
	Matrix(const float FoV, const float near, const float far)
	{
		float s = 1.0f / (tan(FoV * 0.5f * PI / 180.0f) * near);

		float a = -(far / (far - near));
		float b = -((far*near) / (far - near));

		/*
			s, 0, 0, 0
			0, s, 0, 0
			0, 0, a, -1.0
			0, 0, b, 0
		*/
		this->m[0] = s;
		this->m[5] = s;
		this->m[10] = a;
		this->m[15] = 0.0f;

		this->m[11] = -1.0f;
		this->m[14] = b;
	}

	// View
	Matrix(const Vec3 &eye, const Vec3 &target, const Vec3 &up)
	{
		Vec3 zaxis = (eye - target).Normal();			// The "forward" vector.
		Vec3 xaxis = Vec3::Cross(up, zaxis).Normal();	// The "right" vector.
		Vec3 yaxis = Vec3::Cross(zaxis, xaxis);			// The "up" vector.


		this->m[0] = xaxis.x;
		this->m[1] = xaxis.y;
		this->m[2] = xaxis.z;
		this->m[3] = -Vec3::Dot(xaxis, eye);

		this->m[4] = yaxis.x;
		this->m[5] = yaxis.y;
		this->m[6] = yaxis.z;
		this->m[7] = -Vec3::Dot(yaxis, eye);

		this->m[8] = zaxis.x;
		this->m[9] = zaxis.y;
		this->m[10] = zaxis.z;
		this->m[11] = -Vec3::Dot(zaxis, eye);

		this->m[12] = 0;
		this->m[13] = 0;
		this->m[14] = 0;
		this->m[15] = 1;
	}

	// Rotaion
	Matrix(const Vec3 &v, const float degrees)
	{
		float radians = degrees * PI / 180.0f;

		float c = cos(radians);
		float s = sin(radians);

		this->m[0] = c + v.x * v.x * (1.0f - c);
		this->m[1] = v.x * v.y * (1.0f - c) - v.z * s;
		this->m[2] = v.x * v.z * (1.0f - c) + v.y * s;
		this->m[3] = 0.0f;

		this->m[4] = v.y * v.x * (1.0f - c) + v.z * s;
		this->m[5] = c + v.y * v.y * (1.0f - c);
		this->m[6] = v.y * v.z * (1.0f - c) - v.x * s;
		this->m[7] = 0.0f;

		this->m[8] = v.z * v.x * (1.0f - c) - v.y * s;
		this->m[9] = v.z * v.y * (1.0f - c) + v.x * s;
		this->m[10]= c + v.z * v.z * (1.0f - c);
		this->m[11]= 0.0f;

		this->m[12] = 0.0f;
		this->m[13] = 0.0f;
		this->m[14] = 0.0f;
		this->m[15] = 1.0f;
	}

	// From quaternion
	Matrix(const Vec4 &q)
	{
		float x = q.x;
		float y = q.y;
		float z = q.z;
		float w = q.w;

		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
		this->m[0] = 1 - 2*y*y - 2*z*z;
		this->m[1] = 	 2*x*y - 2*z*w;
		this->m[2] = 	 2*x*z + 2*y*w;

		this->m[4] = 	 2*x*y + 2*z*w;
		this->m[5] = 1 - 2*x*x - 2*z*z;
		this->m[6] = 	 2*y*z - 2*x*w;

		this->m[8] = 	 2*x*z - 2*y*w;
		this->m[9] = 	 2*y*z + 2*x*w;
		this->m[10] = 1 - 2*x*x - 2*y*y;
	}


	// Access
	// ------------------------------------------------------------------------
	float Get(const int row, const int col)const
	{
		return this->m[row * 4 + col];
	}

	float& operator[](const int i)
	{
		return this->m[i];
	}

	const float& operator[](const int i)const
	{
		return this->m[i];
	}

	void Set(const int row, const int col, const float value)
	{
		this->m[row * 4 + col] = value;
	}


	// Position
	// ------------------------------------------------------------------------
	void SetPosition(const float x, const float y, const float z)
	{
		this->m[3] = x;
		this->m[7] = y;
		this->m[11] = z;
	}
	void SetPosition(const Vec3 &v)
	{
		this->m[3] = v.x;
		this->m[7] = v.y;
		this->m[11] = v.z;
	}
	void SetPosition(const Vec4 &v)
	{
		this->m[3] = v.x;
		this->m[7] = v.y;
		this->m[11] = v.z;
	}

	Vec3 GetPosition()const
	{
		Vec3 newVector(this->m[3], this->m[7], this->m[11]);

		return newVector;
	}

	void Translate(const float x, const float y, const float z)
	{
		this->m[3] += x;
		this->m[7] += y;
		this->m[11] += z;
	}
	void Translate(const Vec3 &v)
	{
		this->m[3] += v.x;
		this->m[7] += v.y;
		this->m[11] += v.z;
	}
	void Translate(const Vec4 &v)
	{
		this->m[3] += v.x;
		this->m[7] += v.y;
		this->m[11] += v.z;
	}



	// Scale
	// ------------------------------------------------------------------------
	void SetScale(const float x, const float y, const float z)
	{
		this->m[0] = x;
		this->m[5] = y;
		this->m[10] = z;
	}
	void SetScale(const Vec3 &v)
	{
		this->m[0] = v.x;
		this->m[5] = v.y;
		this->m[10] = v.z;
	}
	void SetScale(const Vec4 &v)
	{
		this->m[0] = v.x;
		this->m[5] = v.y;
		this->m[10] = v.z;
	}

	void Scale(const float x, const float y, const float z)
	{
		this->m[0] *= x;
		this->m[5] *= y;
		this->m[10] *= z;
	}
	void Scale(const float scale)
	{
		this->m[0] *= scale;
		this->m[5] *= scale;
		this->m[10] *= scale;
	}



	// Multiplication
	// ------------------------------------------------------------------------
	Matrix operator*(const Matrix &m)const
	{
		Matrix newMatrix;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				float sum = 0;
				for (int inner = 0; inner < 4; inner++)
				{
					sum += this->m[row * 4 + inner] * m.m[inner * 4 + col];
				}
				newMatrix.m[row * 4 + col] = sum;
			}
		}

		return newMatrix;
	}

	Vec4 operator*(const Vec4 &v)const
	{
		Vec4 newVector;

		newVector.x =	this->m[0] * v.x +
						this->m[1] * v.y +
						this->m[2] * v.z +
						this->m[3] * v.w;

		newVector.y =	this->m[4] * v.x +
						this->m[5] * v.y +
						this->m[6] * v.z +
						this->m[7] * v.w;

		newVector.z =	this->m[8] * v.x +
						this->m[9] * v.y +
						this->m[10] * v.z +
						this->m[11] * v.w;

		newVector.w =	this->m[12] * v.x +
						this->m[13] * v.y +
						this->m[14] * v.z +
						this->m[15] * v.w;

		return newVector;
	}

	Vec3 operator*(const Vec3 &v)const
	{
		Vec3 newVector;

		newVector.x =	this->m[0] * v.x +
						this->m[1] * v.y +
						this->m[2] * v.z +
						this->m[3] * 1.0f;

		newVector.y =	this->m[4] * v.x +
						this->m[5] * v.y +
						this->m[6] * v.z +
						this->m[7] * 1.0f;

		newVector.z =	this->m[8] * v.x +
						this->m[9] * v.y +
						this->m[10] * v.z +
						this->m[11] * 1.0f;

		return newVector;
	}



	// Addition
	// ------------------------------------------------------------------------
	Matrix operator+(const Matrix &m)const
	{
		Matrix newMatrix;

		for (int i = 0; i < 16; i++)
		{
			newMatrix[i] = this->m[i] + m.m[i];
		}

		return newMatrix;
	}



	// Equality
	// ------------------------------------------------------------------------
	void operator=(const Matrix &m)
	{
		for (int i = 0; i < 16; i++)
		{
			this->m[i] = m.m[i];
		}
	}

	bool operator==(const Matrix &m)const
	{
		for (int i = 0; i < 16; i++)
		{
			if (this->m[i] != m.m[i])
			{
				return false;
			}
		}

		return true;
	}

	bool operator!=(const Matrix &m)const
	{
		return !this->operator==(m);
	}



	// Static Function
	// ------------------------------------------------------------------------
	static void GetArray(const Matrix &m, float *array)
	{
		for (int i = 0; i < 16; i++)
		{
			array[i] = m.m[i];
		}
	}

	static void Transpose(Matrix &m)
	{
		m = GetTranspose(m);
	}

	static Matrix GetTranspose(const Matrix &m)
	{
		Matrix newMatrix;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				newMatrix.m[row * 4 + col] = m.m[col * 4 + row];
			}
		}

		return newMatrix;
	}

	static void Invert(Matrix &m)
	{
		m = GetInverse(m);
	}

	static Matrix GetInverse(const Matrix &m)
	{
		double Result[4][4];
		double tmp[12]; /* temp array for pairs */
		Matrix src;		/* array of transpose source matrix */
		double det;		/* determinant */
					
		/* transpose matrix */
		src = GetTranspose(m);

		/* calculate pairs for first 8 elements (cofactors) */
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];
		/* calculate first 8 elements (cofactors) */
		Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
		/* calculate pairs for second 8 elements (cofactors) */
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];

		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];
		/* calculate second 8 elements (cofactors) */
		Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
		/* calculate determinant */
		det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];


		Matrix floatResult;

		// Make sure we can get an inverse
		if (det == 0.0f)
			return floatResult;


		/* calculate matrix inverse */
		det = 1.0f / det;

		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 4; col++)
			{
				floatResult[row * 4 + col] = float(Result[row][col] * det);
			}
		}
		
		return floatResult;
	}
};