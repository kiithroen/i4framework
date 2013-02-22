#pragma once

#include "i4core.h"

namespace i4core
{
	class I4Vector3
	{
	public:
		I4Vector3()
		{
		}

		I4Vector3(float v)
		: x(v), y(v), z(v)
		{
		}

		I4Vector3(float nx, float ny, float nz)
		: x(nx), y(ny), z(nz)
		{
		}

		I4Vector3(const I4Vector3& v)
		: x(v.x), y(v.y), z(v.z)
		{
		}


		const I4Vector3& operator = (const I4Vector3& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;

			return *this;
		}

		const I4Vector3 operator - () const
		{
			return I4Vector3(-x, -y, -z);
		}

		void operator += (const I4Vector3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
		}

		void operator -= (const I4Vector3& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
		}

		void operator *= (float val)
		{
			x *= val;
			y *= val;
			z *= val;
		}

		void operator /= (float val)
		{
			float inv = 1.0f / val;
			x *= inv;
			y *= inv;
			z *= inv;
		}

		const I4Vector3 operator + (const I4Vector3& rhs) const
		{
			return I4Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		const I4Vector3 operator - (const I4Vector3& rhs) const
		{
			return I4Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
		}
		
		const I4Vector3 operator * (const I4Vector3& rhs) const
		{
			return I4Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
		}

		const I4Vector3 operator * (float val) const
		{
			return I4Vector3(x*val, y*val, z*val);
		}

		const I4Vector3 operator / (float val) const
		{
			float inv = 1.0f / val;
			return I4Vector3(x*inv, y*inv, z*inv);
		}

		bool operator == (const I4Vector3& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
		}

		bool operator != (const I4Vector3& rhs) const
		{
			return !(*this == rhs);
		}

		void set(float nx, float ny, float nz)
		{
			x = nx;
			y = ny;
			z = nz;
		}

		void invert()
		{
			x = -x;
			y = -y;
			z = -z;
		}

		void normalize()
		{
			float inv = 1.0f / getLength();
			x *= inv;
			y *= inv;
			z *= inv;
		}

		float getLength() const
		{
			return sqrtf(getLengthSq());
		}

		float getLengthSq() const
		{
			return (x*x + y*y + z*z);
		}

	public:
		static float distance(const I4Vector3& a, const I4Vector3& b)
		{
			return sqrtf(distanceSq(a, b));
		}

		static float distanceSq(const I4Vector3& a, const I4Vector3& b)
		{
			float _x = a.x - b.x;
			float _y = a.y - b.y;
			float _z = a.z - b.z;
			return _x*_x + _y*_y + _z*_z;
		}

		static float dotProduct(const I4Vector3& a, const I4Vector3& b)
		{
			return (a.x*b.x + a.y*b.y + a.z*b.z);
		}

		static const I4Vector3 crossProduct(const I4Vector3& a, const I4Vector3& b)
		{
			return I4Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
		}

		static float angle(const I4Vector3& a, const I4Vector3& b)
		{
			return acos(dotProduct(a, b)/(a.getLength()*b.getLength()));
		}


	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};
			
			float xyz[3];
		};
	};

	inline const I4Vector3 operator * (float val, const I4Vector3& rhs)
	{
		return I4Vector3(rhs.x*val, rhs.y*val, rhs.z*val);
	}

	const I4Vector3 I4VECTOR3_AXISX = I4Vector3(1, 0, 0);
	const I4Vector3 I4VECTOR3_AXISY = I4Vector3(0, 1, 0);
	const I4Vector3 I4VECTOR3_AXISZ = I4Vector3(0, 0, 1);

	const I4Vector3 I4VECTOR3_ZERO = I4Vector3(0, 0, 0);
	const I4Vector3 I4VECTOR3_HALF = I4Vector3(0.5f, 0.5f, 0.5f);
	const I4Vector3 I4VECTOR3_ONE = I4Vector3(1, 1, 1);
}


