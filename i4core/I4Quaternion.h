#pragma once

#include "I4MathUtil.h"
#include "I4Matrix4x4.h"

namespace i4core
{
	class I4Quaternion
	{
	public:
		I4Quaternion()
		{
		}

		I4Quaternion(float nx, float ny, float nz, float nw)
			: x(nx), y(ny), z(nz), w(nw)
		{
		}

		I4Quaternion(const I4Quaternion& q)
			: x(q.x), y(q.y), z(q.z), w(q.w)
		{
		}

		const I4Quaternion& operator = (const I4Quaternion& rhs)
		{
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;
			w = rhs.w;

			return *this;
		}

		const I4Quaternion operator ~ () const	// ÄÓ·¹ »ç¿ø¼ö
		{
			return I4Quaternion(-x, -y, -z, w);
		}

		void operator += (const I4Quaternion& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			w += rhs.w;
		}

		void operator -= (const I4Quaternion& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			w -= rhs.w;
		}

		void operator *= (const I4Quaternion& rhs)
		{
			I4Quaternion result;
			multiply(result, *this, rhs);
			*this = result;
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

		const I4Quaternion operator + (const I4Quaternion& rhs) const
		{
			return I4Quaternion(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
		}

		const I4Quaternion operator - (const I4Quaternion& rhs) const
		{
			return I4Quaternion(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
		}

		const I4Quaternion operator * (const I4Quaternion& rhs) const
		{
			I4Quaternion result;
			multiply(result, *this, rhs);
			return result;	
		}

		I4Quaternion operator * (const I4Vector3& v) const
		{
			return I4Quaternion(w*v.x + y*v.z - z*v.y,
				w*v.y + z*v.x - x*v.z,
				w*v.z + x*v.y - y*v.x,
				-(x*v.x + y*v.y + z*v.z));
		}

		const I4Quaternion operator * (float val) const
		{
			return I4Quaternion(x*val, y*val, z*val, w*val);
		}

		const I4Quaternion operator / (float val) const
		{
			float inv = 1.0f / val;
			return I4Quaternion(x*inv, y*inv, z*inv, w*inv);
		}

		
		bool operator == (const I4Quaternion& rhs) const
		{
			return (x == rhs.x) && (y == rhs.y) && (z == rhs.z) && (w == rhs.z);
		}

		bool operator != (const I4Quaternion& rhs) const
		{
			return !(*this == rhs);
		}

		void normalize()
		{
			float inv = 1/sqrtf(x*x + y*y + z*z + w*w);

			x *= inv;
			y *= inv;
			z *= inv;
			w *= inv;
		}
		
		const I4Vector3 transform(const I4Vector3& v)
		{
			I4Quaternion result = (*this)*v*(~*this);
			return I4Vector3(result.x, result.y, result.z);
		}

		void makeIdentity()
		{
			x = y = z = 0.0f;
			w = 1.0f;
		}

		void makeRotationAxis(const I4Vector3& inAxis, float fAngle)
		{
			I4Vector3 axis = inAxis;
			axis.normalize();

			float halfAngle = fAngle*0.5f;

			float s = sin(halfAngle);
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
			w = cos(halfAngle);

		}

		void makeRotationYawPitchRoll(float yaw, float Pitch, float roll)
		{		
			float halfYaw   = yaw*0.5f;
			float halfPitch = Pitch*0.5f;
			float halffRoll = roll*0.5f;

			float cX = cos(halfPitch);
			float cY = cos(halfYaw);
			float cZ = cos(halffRoll);

			float sX = sin(halfPitch);
			float sY = sin(halfYaw);
			float sZ = sin(halffRoll);

			float cYcZ = cY * cZ;
			float sYsZ = sY * sZ;
			float cYsZ = cY * sZ;
			float sYcZ = sY * cZ;

			x = sX * cYcZ + cX * sYsZ;
			y = cX * sYcZ - sX * cYsZ;
			z = cX * cYsZ - sX * sYcZ;
			w = cX * cYcZ + sX * sYsZ;
		}

		void makeRotationMatrix(const I4Matrix4x4& mat)
		{
			const int next[3] = { 1, 2, 0 };

			float trace = mat._11 + mat._22 + mat._33;
			float root;

			if (trace > 0.0f)
			{
				root = sqrtf(trace + 1.0f);
				w = 0.5f*root;
				root = 0.5f/root;
				x = (mat._23 - mat._32)*root;
				y = (mat._31 - mat._13)*root;
				z = (mat._12 - mat._21)*root;
			}
			else
			{
				int i = 0;
				if (mat._22 > mat._11)
				{
					i = 1;
				}
				if (mat._33 > mat.m[i][i])
				{
					i = 2;
				}
				int j = next[i];
				int k = next[j];

				root = sqrtf(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0f);
				float* quat[3] = { &x, &y, &z };
				*quat[i] = 0.5f*root;
				root = 0.5f/root;
				w = (mat.m[k][j] - mat.m[j][k])*root;
				*quat[j] = (mat.m[j][i] + mat.m[i][j])*root;
				*quat[k] = (mat.m[k][i] + mat.m[i][k])*root;
			}
		}

		void extractYawPitchRoll(float& yaw, float& pitch, float& roll) const
		{
			float test = x*y + z*w;
			if (test > 0.499)
			{
				yaw = 2.0f * atan2f(x,w);
				roll = I4PI/2;
				pitch = 0.0f;
				return;
			}
			if (test < -0.499)
			{
				yaw = -2.0f * atan2f(x,w);
				roll = -I4PI/2;
				pitch = 0.0f;
				return;
			}
			float sqx = x*x;
			float sqy = y*y;
			float sqz = z*z;
			yaw = atan2f(2.0f*y*w-2.0f*x*z , 1.0f - 2.0f*sqy - 2.0f*sqz);
			roll = asinf(2.0f*test);
			pitch = atan2f(2.0f*x*w-2.0f*y*z , 1.0f - 2.0f*sqx - 2.0f*sqz);
		}

		void extractMatrix(I4Matrix4x4& mat) const
		{
			float x2 = x + x;
			float y2 = y + y;
			float z2 = z + z;
			float xx = x*x2;
			float xy = x*y2;
			float xz = x*z2;
			float yy = y*y2;
			float yz = y*z2;
			float zz = z*z2;
			float wx = w*x2;
			float wy = w*y2;
			float wz = w*z2;

			mat._11 = 1.0f - (yy + zz);
			mat._21 = xy - wz;
			mat._31 = xz + wy;
			mat._12 = xy + wz;
			mat._22 = 1.0f - (xx + zz);
			mat._32 = yz - wx;
			mat._13 = xz - wy;
			mat._23 = yz + wx;
			mat._33 = 1.0f - (xx + yy);
			mat._14 = mat._24 = mat._34 = mat._41 = mat._42 = mat._43 = 0.0f;
			mat._44 = 1.0f;
		}

	public:
		float	x;
		float	y;
		float	z;
		float	w;

	public:
		static void multiply(I4Quaternion& out, const I4Quaternion& a, const I4Quaternion& b)
		{
			assert(&out != &a);
			assert(&out != &b);

			out.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
			out.y = a.w*b.y + a.y*b.w + a.z*b.x - a.x*b.z;
			out.z = a.w*b.z + a.z*b.w + a.x*b.y - a.y*b.x;
			out.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
		}

		static void slerp(I4Quaternion& out, const I4Quaternion& _a, const I4Quaternion& _b, float t)
		{
			I4Quaternion a = _a;
			I4Quaternion b = _b;

			float angle = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;

			if (angle < 0.0f)
			{
				a *= -1.0f;
				angle *= -1.0f;
			}

			float scale;
			float invscale;

			if ((angle + 1.0f) > 0.05f)
			{
				if ((1.0f - angle) >= 0.05f)
				{
					float theta = acosf(angle);
					float invsintheta = 1.0f / sinf(theta);
					scale = sinf(theta * (1.0f-t)) * invsintheta;
					invscale = sinf(theta * t) * invsintheta;
				}
				else
				{
					scale = 1.0f - t;
					invscale = t;
				}
			}
			else
			{
				b.x = -a.y;
				b.y = a.x;
				b.z = -a.w;
				b.w = a.z;

				scale = sinf(I4PI * (0.5f - t));
				invscale = sinf(I4PI * t);
			}

			out = (a*scale) + (b*invscale);
		}
	};
}