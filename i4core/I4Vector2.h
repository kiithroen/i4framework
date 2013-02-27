#pragma once

namespace i4core
{
	class I4Vector2
	{
	public:
		I4Vector2()
		{
		}

		I4Vector2(float nx, float ny)
			: x(nx), y(ny)
		{
		}

		I4Vector2(const I4Vector2& v)
			: x(v.x), y(v.y)
		{
		}


		const I4Vector2& operator = (const I4Vector2& rhs)
		{
			x = rhs.x;
			y = rhs.y;

			return *this;
		}

		const I4Vector2 operator - () const
		{
			return I4Vector2(-x, -y);
		}

		void operator += (const I4Vector2& rhs)
		{
			x += rhs.x;
			y += rhs.y;
		}

		void operator -= (const I4Vector2& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
		}

		void operator *= (float val)
		{
			x *= val;
			y *= val;
		}

		void operator /= (float val)
		{
			float inv = 1.0f / val;
			x *= inv;
			y *= inv;
		}

		const I4Vector2 operator + (const I4Vector2& rhs) const
		{
			return I4Vector2(x + rhs.x, y + rhs.y);
		}

		const I4Vector2 operator - (const I4Vector2& rhs) const
		{
			return I4Vector2(x - rhs.x, y - rhs.y);
		}

		const I4Vector2 operator * (const I4Vector2& rhs) const
		{
			return I4Vector2(x*rhs.x, y*rhs.y);
		}

		const I4Vector2 operator * (float val) const
		{
			return I4Vector2(x*val, y*val);
		}

		const I4Vector2 operator / (float val) const
		{
			float inv = 1.0f / val;
			return I4Vector2(x*inv, y*inv);
		}

		bool operator == (const I4Vector2& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y);
		}

		bool operator != (const I4Vector2& rhs) const
		{
			return !(*this == rhs);
		}

		void set(float nx, float ny)
		{
			x = nx;
			y = ny;
		}

		void invert()
		{
			x = -x;
			y = -y;
		}

	public:
		union
		{
			struct
			{
				float x;
				float y;
			};


			float xy[2];
		};
	};

	inline const I4Vector2 operator * (float val, const I4Vector2& rhs)
	{
		return I4Vector2(rhs.x*val, rhs.y*val);
	}
}