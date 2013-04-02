#pragma once

namespace i4core
{
	class Vector3
	{
	public:
		Vector3()
		{
		}

		Vector3(float v)
		: x(v), y(v), z(v)
		{
		}

		Vector3(float nx, float ny, float nz)
		: x(nx), y(ny), z(nz)
		{
		}

		Vector3(const Vector3& v)
		: x(v.x), y(v.y), z(v.z)
		{
		}


		const Vector3& operator = (const Vector3& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;

			return *this;
		}

		const Vector3 operator - () const
		{
			return Vector3(-x, -y, -z);
		}

		void operator += (const Vector3& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
		}

		void operator -= (const Vector3& rhs)
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

		const Vector3 operator + (const Vector3& rhs) const
		{
			return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
		}

		const Vector3 operator - (const Vector3& rhs) const
		{
			return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
		}
		
		const Vector3 operator * (const Vector3& rhs) const
		{
			return Vector3(x*rhs.x, y*rhs.y, z*rhs.z);
		}

		const Vector3 operator * (float val) const
		{
			return Vector3(x*val, y*val, z*val);
		}

		const Vector3 operator / (float val) const
		{
			float inv = 1.0f / val;
			return Vector3(x*inv, y*inv, z*inv);
		}

		bool operator == (const Vector3& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z);
		}

		bool operator != (const Vector3& rhs) const
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
		static float distance(const Vector3& a, const Vector3& b)
		{
			return sqrtf(distanceSq(a, b));
		}

		static float distanceSq(const Vector3& a, const Vector3& b)
		{
			float _x = a.x - b.x;
			float _y = a.y - b.y;
			float _z = a.z - b.z;
			return _x*_x + _y*_y + _z*_z;
		}

		static float dotProduct(const Vector3& a, const Vector3& b)
		{
			return (a.x*b.x + a.y*b.y + a.z*b.z);
		}

		static const Vector3 crossProduct(const Vector3& a, const Vector3& b)
		{
			return Vector3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
		}

		static float angle(const Vector3& a, const Vector3& b)
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

	inline const Vector3 operator * (float val, const Vector3& rhs)
	{
		return Vector3(rhs.x*val, rhs.y*val, rhs.z*val);
	}

	const Vector3 VECTOR3_AXISX = Vector3(1, 0, 0);
	const Vector3 VECTOR3_AXISY = Vector3(0, 1, 0);
	const Vector3 VECTOR3_AXISZ = Vector3(0, 0, 1);

	const Vector3 VECTOR3_ZERO = Vector3(0, 0, 0);
	const Vector3 VECTOR3_HALF = Vector3(0.5f, 0.5f, 0.5f);
	const Vector3 VECTOR3_ONE = Vector3(1, 1, 1);
}


