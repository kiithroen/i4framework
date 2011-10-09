#pragma once

#include "i4core.h"

namespace i4core
{
	class I4Vector4
	{
	public:
		I4Vector4()
		{
		}

		I4Vector4(float nx, float ny, float nz, float nw)
		: x(nx), y(ny), z(nz), w(nw)
		{
		}

		I4Vector4(const I4Vector4& v)
		: x(v.x), y(v.y), z(v.z), w(v.w)
		{
		}


		const I4Vector4& operator = (const I4Vector4& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;

			return *this;
		}

		const I4Vector4 operator - () const
		{
			return I4Vector4(-x, -y, -z, -w);
		}

		void operator += (const I4Vector4& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
		}

		void operator -= (const I4Vector4& rhs)
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

		const I4Vector4 operator + (const I4Vector4& rhs) const
		{
			return I4Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		const I4Vector4 operator - (const I4Vector4& rhs) const
		{
			return I4Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}
		
		const I4Vector4 operator * (const I4Vector4& rhs) const
		{
			return I4Vector4(x*rhs.x, y*rhs.y, z*rhs.z, w*rhs.w);
		}

		const I4Vector4 operator * (float val) const
		{
			return I4Vector4(x*val, y*val, z*val, w*val);
		}

		const I4Vector4 operator / (float val) const
		{
			float inv = 1.0f / val;
			return I4Vector4(x*inv, y*inv, z*inv, w*inv);
		}

		bool operator == (const I4Vector4& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.w);
		}

		bool operator != (const I4Vector4& rhs) const
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
			
			float xyzw[4];
		};
	};

	inline const I4Vector4 operator * (float val, const I4Vector4& rhs)
	{
		return I4Vector4(rhs.x*val, rhs.y*val, rhs.z*val, rhs.w*val);
	}

}