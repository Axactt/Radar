#pragma once

#include<cmath>
#include <corecrt_math_defines.h> // For PI cosnstant
class Vector2
{
public:
	float m_x {};
	float m_y {};

	Vector2() = default;
	Vector2( float x, float y )
		:m_x { x }, m_y { y }
	{
	}
	Vector2& operator=( const Vector2& vec )
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
	}

	Vector2& operator+=( const Vector2& vec )
	{
		m_x += vec.m_x;
		m_y += vec.m_y;
		return *this;
	}
	Vector2& operator-=( const Vector2& vec )
	{
		m_x -= vec.m_x;
		m_y -= vec.m_y;
		return *this;
	}
	Vector2& operator*=( const Vector2& vec )
	{
		m_x *= vec.m_x;
		m_y *= vec.m_y;
		return *this;
	}
	Vector2& operator/=( const Vector2& vec )
	{
		m_x /= vec.m_x;
		m_y /= vec.m_y;
		return *this;
	}
	Vector2& operator*=( const float f )
	{
		m_x *= f;
		m_y *= f;
		return *this;
	}
	Vector2& operator/=( const float f )
	{
		m_x /= f;
		m_y /= f;

		return *this;
	}

	friend Vector2 operator+( const Vector2& vec1, const Vector2& vec2 )
	{
		return { vec1.m_x + vec2.m_x , vec1.m_y + vec2.m_y };
	}
	friend Vector2 operator-( const Vector2& vec1, const Vector2& vec2 )
	{
		return{ vec1.m_x - vec2.m_x,vec1.m_y - vec2.m_y };
	}

	float Length() const
	{
		return sqrt( (m_x * m_x) + (m_y * m_y) );
	}

	Vector2& Normalize()
	{
		return { (*this) *= (1 / Length()) };
	}
	float DistanceTo( const Vector2& vec )
	{
		return(*this - vec).Length();
	}
	float Dot( const Vector2& vec )
	{
		return { (m_x * vec.m_x) + (m_y * vec.m_y) };
	}

	friend Vector2 RotatePoint( Vector2 pointToRotate, Vector2 centrePoint, float angle, bool angleInRadians = false )
	{
		if (!angleInRadians)
			angle = (float) (angle * M_PI / 180.0f);
		float cosTheta = (float) cos( angle );
		float sineTheta = (float) sin( angle );
		Vector2 returnVec = {
			cosTheta * (pointToRotate.m_x - centrePoint.m_x) - sineTheta * (pointToRotate.m_y - centrePoint.m_y),
			sineTheta * (pointToRotate.m_x - centrePoint.m_x) + cosTheta * (pointToRotate.m_y - centrePoint.m_y) };
		returnVec += centrePoint;
		return returnVec;
	}

};

class Vector3
{
	
public:
	float m_x {};
	float m_y {};
	float m_z {};
	Vector3() = default;
	Vector3( float x, float y, float z )
		:m_x { x }, m_y { y }, m_z { z }
	{
	}
	Vector3& operator=( const Vector3& vec )
	{
		m_x = vec.m_x;
		m_y = vec.m_y;
		m_z = vec.m_z;
	}

	Vector3& operator+=( const Vector3& vec )
	{
		m_x += vec.m_x;
		m_y += vec.m_y;
		m_z +=vec.m_z;
		return *this;
	}
	Vector3& operator-=( const Vector3& vec )
	{
		m_x -= vec.m_x;
		m_y -= vec.m_y;
		m_z -= vec.m_z;
		return *this;
	}
	Vector3& operator*=( const Vector3& vec )
	{
		m_x *= vec.m_x;
		m_y *= vec.m_y;
		m_z *= vec.m_z;
		return *this;
	}
	Vector3& operator/=( const Vector3& vec )
	{
		m_x /= vec.m_x;
		m_y /= vec.m_y;
		m_z /= vec.m_z;
		return *this;
	}
	Vector3& operator*=( const float f )
	{
		m_x *= f;
		m_y *= f;
		m_z *= f;
		return *this;
	}
	Vector3& operator/=( const float f )
	{
		m_x /= f;
		m_y /= f;
		m_z /= f;
		return *this;
	}

	friend Vector3 operator+( const Vector3& vec1, const Vector3& vec2 )
	{
		return{ vec1.m_x + vec2.m_x,vec1.m_y + vec2.m_y,vec1.m_z + vec2.m_z };
	}
	friend Vector3 operator-( const Vector3& vec1, const Vector3& vec2 )
	{
		return{ vec1.m_x - vec2.m_x,vec1.m_y - vec2.m_y,vec1.m_z - vec2.m_z };
	}

	float Length() const
	{
		return sqrt( (m_x*m_x) + (m_y*m_y) + (m_z*m_z) );
	}
	
	Vector3& Normalize()
	{
		return { (*this) *= (1 / Length()) };
	}
	float DistanceTo( const Vector3& vec )
	{
		return(*this - vec).Length();
	}
	float Dot( const Vector3& vec )
	{
		return { (m_x * vec.m_x) + (m_y * vec.m_y) + (m_z * vec.m_z) };
	}
	Vector3 Cross( const Vector3& vec ) const
	{
		return { ((m_y * vec.m_z) - (m_z * vec.m_y)), ((m_z * vec.m_x) - (m_x * vec.m_z)), ((m_x * vec.m_y) - (m_y * vec.m_x)) };
	}
};