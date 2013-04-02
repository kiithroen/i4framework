#pragma once

namespace i4core
{
	class Vector2
	{
	public:
		Vector2()
		{
		}

		Vector2(float nx, float ny)
			: x(nx), y(ny)
		{
		}

		Vector2(const Vector2& v)
			: x(v.x), y(v.y)
		{
		}


		const Vector2& operator = (const Vector2& rhs)
		{
			x = rhs.x;
			y = rhs.y;

			return *this;
		}

		const Vector2 operator - () const
		{
			return Vector2(-x, -y);
		}

		void operator += (const Vector2& rhs)
		{
			x += rhs.x;
			y += rhs.y;
		}

		void operator -= (const Vector2& rhs)
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

		const Vector2 operator + (const Vector2& rhs) const
		{
			return Vector2(x + rhs.x, y + rhs.y);
		}

		const Vector2 operator - (const Vector2& rhs) const
		{
			return Vector2(x - rhs.x, y - rhs.y);
		}

		const Vector2 operator * (const Vector2& rhs) const
		{
			return Vector2(x*rhs.x, y*rhs.y);
		}

		const Vector2 operator * (float val) const
		{
			return Vector2(x*val, y*val);
		}

		const Vector2 operator / (float val) const
		{
			float inv = 1.0f / val;
			return Vector2(x*inv, y*inv);
		}

		bool operator == (const Vector2& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y);
		}

		bool operator != (const Vector2& rhs) const
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

	inline const Vector2 operator * (float val, const Vector2& rhs)
	{
		return Vector2(rhs.x*val, rhs.y*val);
	}
}