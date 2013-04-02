#pragma once

namespace i4core
{
	class Vector4
	{
	public:
		Vector4()
		{
		}

		Vector4(float nx, float ny, float nz, float nw)
		: x(nx), y(ny), z(nz), w(nw)
		{
		}

		Vector4(const Vector4& v)
		: x(v.x), y(v.y), z(v.z), w(v.w)
		{
		}


		const Vector4& operator = (const Vector4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;

			return *this;
		}

		const Vector4 operator - () const
		{
			return Vector4(-x, -y, -z, -w);
		}

		void operator += (const Vector4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
		}

		void operator -= (const Vector4& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
		}

		void operator *= (float val)
		{
			x *= val;
			y *= val;
			z *= val;
			w *= val;
		}

		void operator /= (float val)
		{
			float inv = 1.0f / val;
			x *= inv;
			y *= inv;
			z *= inv;
			w *= inv;
		}

		const Vector4 operator + (const Vector4& rhs) const
		{
			return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		const Vector4 operator - (const Vector4& rhs) const
		{
			return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}
		
		const Vector4 operator * (const Vector4& rhs) const
		{
			return Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
		}

		const Vector4 operator * (float val) const
		{
			return Vector4(x*val, y*val, z*val, w*val);
		}

		const Vector4 operator / (float val) const
		{
			float inv = 1.0f / val;
			return Vector4(x*inv, y*inv, z*inv, w*inv);
		}

		bool operator == (const Vector4& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
		}

		bool operator != (const Vector4& rhs) const
		{
			return !(*this == rhs);
		}
		
		void set(float nx, float ny, float nz, float nw)
		{
			x = nx;
			y = ny;
			z = nz;
			w = nw;
		}

		void invert()
		{
			x = -x;
			y = -y;
			z = -z;
			w = -w;
		}

	public:
		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
			
			struct
			{
				float r;
				float g;
				float b;
				float a;
			};

			float xyzw[4];
		};
	};

	inline const Vector4 operator * (float val, const Vector4& rhs)
	{
		return Vector4(rhs.x*val, rhs.y*val, rhs.z*val, rhs.w*val);
	}

}