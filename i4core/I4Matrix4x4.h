#pragma once

#include "i4core.h"
#include "I4Vector3.h"
#include "I4MathUtil.h"

namespace i4core
{
	class I4Matrix4x4
	{
	public:
		I4Matrix4x4()
		{
		}

		I4Matrix4x4(float m11, float m12, float m13, float m14,
				float m21, float m22, float m23, float m24,
				float m31, float m32, float m33, float m34,
				float m41, float m42, float m43, float m44)
		: _11(m11), _12(m12), _13(m13), _14(m14)
		, _21(m21), _22(m22), _23(m23), _24(m24)
		, _31(m31), _32(m32), _33(m33), _34(m34)
		, _41(m41), _42(m42), _43(m43), _44(m44)
		{		
		}

		I4Matrix4x4(const I4Matrix4x4& m)
		: _11(m._11), _12(m._12), _13(m._13), _14(m._14)
		, _21(m._21), _22(m._22), _23(m._23), _24(m._24)
		, _31(m._31), _32(m._32), _33(m._33), _34(m._34)
		, _41(m._41), _42(m._42), _43(m._43), _44(m._44)
		{		
		}

		const I4Matrix4x4& operator = (const I4Matrix4x4& rhs)
		{
			memcpy(&m, &rhs.m, sizeof(I4Matrix4x4));

			return *this;
		}

		const I4Matrix4x4 operator - () const
		{
			return I4Matrix4x4(-_11, -_12, -_13, -_14,
						   -_21, -_22, -_23, -_24,
						   -_31, -_32, -_33, -_34,
						   -_41, -_42, -_43, -_44);
		}
			
		void operator += (const I4Matrix4x4& rhs)
		{
			_11 += rhs._11; _12 += rhs._12; _13 += rhs._13; _14 += rhs._14;
			_21 += rhs._21; _22 += rhs._22; _23 += rhs._23; _24 += rhs._24;
			_31 += rhs._31; _32 += rhs._32; _33 += rhs._33; _34 += rhs._34;
			_41 += rhs._41; _42 += rhs._42; _43 += rhs._43; _44 += rhs._44;
		}

		void operator -= (const I4Matrix4x4& rhs)
		{
			_11 -= rhs._11; _12 -= rhs._12; _13 -= rhs._13; _14 -= rhs._14;
			_21 -= rhs._21; _22 -= rhs._22; _23 -= rhs._23; _24 -= rhs._24;
			_31 -= rhs._31; _32 -= rhs._32; _33 -= rhs._33; _34 -= rhs._34;
			_41 -= rhs._41; _42 -= rhs._42; _43 -= rhs._43; _44 -= rhs._44;				
		}

		void operator *= (const I4Matrix4x4& rhs)
		{
			I4Matrix4x4 result;
			multiply(result, *this, rhs);
			*this = result;
		}

		void operator *= (float val)
		{
			_11 *= val; _12 *= val; _13 *= val; _14 *= val;
			_21 *= val; _22 *= val; _23 *= val; _24 *= val;
			_31 *= val; _32 *= val; _33 *= val; _34 *= val;
			_41 *= val; _42 *= val; _43 *= val; _44 *= val;
		}

		void operator /= (float val)
		{
			float inv = 1.0f / val;

			_11 *= inv; _12 *= inv; _13 *= inv; _14 *= inv;
			_21 *= inv; _22 *= inv; _23 *= inv; _24 *= inv;
			_31 *= inv; _32 *= inv; _33 *= inv; _34 *= inv;
			_41 *= inv; _42 *= inv; _43 *= inv; _44 *= inv;
		}

		const I4Matrix4x4 operator + (const I4Matrix4x4& rhs) const
		{
			return I4Matrix4x4(_11 + rhs._11, _12 + rhs._12, _13 + rhs._13, _14 + rhs._14,
						   _21 + rhs._21, _22 + rhs._22, _23 + rhs._23, _24 + rhs._24,
						   _31 + rhs._31, _32 + rhs._32, _33 + rhs._33, _34 + rhs._34,
						   _41 + rhs._41, _42 + rhs._42, _43 + rhs._43, _44 + rhs._44);
		}

		const I4Matrix4x4 operator - (const I4Matrix4x4& rhs) const
		{
			return I4Matrix4x4(_11 - rhs._11, _12 - rhs._12, _13 - rhs._13, _14 - rhs._14,
						   _21 - rhs._21, _22 - rhs._22, _23 - rhs._23, _24 - rhs._24,
						   _31 - rhs._31, _32 - rhs._32, _33 - rhs._33, _34 - rhs._34,
						   _41 - rhs._41, _42 - rhs._42, _43 - rhs._43, _44 - rhs._44);
		}

		const I4Matrix4x4 operator * (const I4Matrix4x4& rhs) const
		{
			I4Matrix4x4 result;
			multiply(result, *this, rhs);

			return result;		
		}

		const I4Matrix4x4 operator * (float val) const
		{
			return I4Matrix4x4(_11*val, _12*val, _13*val, _14*val,
						   _21*val, _22*val, _23*val, _24*val,
						   _31*val, _32*val, _33*val, _34*val,
						   _41*val, _42*val, _43*val, _44*val);
		}

		const I4Matrix4x4 operator / (float val) const
		{
			float inv = 1.0f / val;

			return I4Matrix4x4(_11*inv, _12*inv, _13*inv, _14*inv,
						   _21*inv, _22*inv, _23*inv, _24*inv,
						   _31*inv, _32*inv, _33*inv, _34*inv,
						   _41*inv, _42*inv, _43*inv, _44*inv);
		}

		void setAxisX(const I4Vector3& v)
		{
			_11 = v.x;
			_12 = v.y;
			_13 = v.z;
		}

		void extractAxisX(I4Vector3& v) const
		{
			v.x = _11;
			v.y = _12;
			v.z = _13;
		}

		void setAxisY(const I4Vector3& v)
		{
			_21 = v.x;
			_22 = v.y;
			_23 = v.z;
		}

		void extractAxisY(I4Vector3& v) const
		{
			v.x = _21;
			v.y = _22;
			v.z = _23;
		}

		void setAxisZ(const I4Vector3& v)
		{
			_31 = v.x;
			_32 = v.y;
			_33 = v.z;
		}

		void extractAxisZ(I4Vector3& v) const
		{
			v.x = _31;
			v.y = _32;
			v.z = _33;
		}

		void setTranslation(const I4Vector3& pos)
		{
			_41 = pos.x;
			_42 = pos.y;
			_43 = pos.z;
		}

		void extractTranslation(I4Vector3& v) const
		{
			v.x = _41;
			v.y = _42;
			v.z = _43;
		}

		void extractAxis(I4Vector3& vx, I4Vector3& vy, I4Vector3& vz) const
		{
			extractAxisX(vx);
			extractAxisY(vy);
			extractAxisZ(vz);
		}

		
		void extractInversePrimitive(I4Matrix4x4& mat) const
		{
			// 회전 이동 반사로만 구성된 행렬일때 역행렬은 다음과 같다.

			// nx ny nz 0           nx   ox   ax   0
			// ox oy oz 0    -1     ny   oy   ay   0
			// ax ay az 0  ----->   nz   oz   az   0
			// px py pz 1          -p.n -p.o -p.z  1

			mat._11 = _11;
			mat._12 = _21;
			mat._13 = _31;
			mat._21 = _12;
			mat._22 = _22;
			mat._23 = _32;
			mat._31 = _13;
			mat._32 = _23;
			mat._33 = _33;
			mat._41 = -(_41*_11 + _42*_12 + _43*_13);
			mat._42 = -(_41*_21 + _42*_22 + _43*_23);
			mat._43 = -(_41*_31 + _42*_32 + _43*_33);
			
			mat._14 = mat._24 = mat._34 = 0.0f;
			mat._44 = 1.0f;
		}

		bool extractInverse(I4Matrix4x4& mat) const
		{
			float d = (_11*_22 - _12*_21)*(_33*_44 - _34*_43) -
						(_11*_23 - _13*_21)*(_32*_44 - _34*_42) +
						(_11*_24 - _14*_21)*(_32*_43 - _33*_42) +
						(_12*_23 - _13*_22)*(_31*_44 - _34*_41) -
						(_12*_24 - _14*_22)*(_31*_43 - _33*_41) +
						(_13*_24 - _14*_23)*(_31*_42 - _32*_41);

			if (mathutil::isEpsilonZero(d))
				return false;

			float invD = 1.0f/d;

			mat._11 = invD*(_22*(_33*_44 - _34*_43) + _23*(_34*_42 - _32*_44) +	_24*(_32*_43 - _33*_42));
			mat._12 = invD*(_32*(_13*_44 - _14*_43) + _33*(_14*_42 - _12*_44) + _34*(_12*_43 - _13*_42));
			mat._13 = invD*(_42*(_13*_24 - _14*_23) + _43*(_14*_22 - _12*_24) + _44*(_12*_23 - _13*_22));
			mat._14 = invD*(_12*(_24*_33 - _23*_34) + _13*(_22*_34 - _24*_32) + _14*(_23*_32 - _22*_33));
			mat._21 = invD*(_23*(_31*_44 - _34*_41) + _24*(_33*_41 - _31*_43) + _21*(_34*_43 - _33*_44));
			mat._22 = invD*(_33*(_11*_44 - _14*_41) + _34*(_13*_41 - _11*_43) + _31*(_14*_43 - _13*_44));
			mat._23 = invD*(_43*(_11*_24 - _14*_21) + _44*(_13*_21 - _11*_23) + _41*(_14*_23 - _13*_24));
			mat._24 = invD*(_13*(_24*_31 - _21*_34) + _14*(_21*_33 - _23*_31) + _11*(_23*_34 - _24*_33));
			mat._31 = invD*(_24*(_31*_42 - _32*_41) + _21*(_32*_44 - _34*_42) + _22*(_34*_41 - _31*_44));
			mat._32 = invD*(_34*(_11*_42 - _12*_41) + _31*(_12*_44 - _14*_42) + _32*(_14*_41 - _11*_44));
			mat._33 = invD*(_44*(_11*_22 - _12*_21) + _41*(_12*_24 - _14*_22) + _42*(_14*_21 - _11*_24));
			mat._34 = invD*(_14*(_22*_31 - _21*_32) + _11*(_24*_32 - _22*_34) + _12*(_21*_34 - _24*_31));
			mat._41 = invD*(_21*(_33*_42 - _32*_43) + _22*(_31*_43 - _33*_41) + _23*(_32*_41 - _31*_42));
			mat._42 = invD*(_31*(_13*_42 - _12*_43) + _32*(_11*_43 - _13*_41) + _33*(_12*_41 - _11*_42));
			mat._43 = invD*(_41*(_13*_22 - _12*_23) + _42*(_11*_23 - _13*_21) + _43*(_12*_21 - _11*_22));
			mat._44 = invD*(_11*(_22*_33 - _23*_32) + _12*(_23*_31 - _21*_33) + _13*(_21*_32 - _22*_31));

			return true;
		}

		void makeTranspose(const I4Matrix4x4& mat)
		{
			_11 = mat._11;
			_12 = mat._21;
			_13 = mat._31;
			_14 = mat._41;
			_21 = mat._12;
			_22 = mat._22;
			_23 = mat._32;
			_24 = mat._42;
			_31 = mat._13;
			_32 = mat._23;
			_33 = mat._33;
			_34 = mat._43;
			_41 = mat._14;
			_42 = mat._24;
			_43 = mat._34;
			_44 = mat._44;
		}

		void makeZero()
		{
			memset(arr, 0, sizeof(I4Matrix4x4));			
		}

		void makeIdentity()
		{
			makeZero();

			_11 = _22 = _33 = _44 = 1.0f;
		}
		
		void makeTranslation(float x, float y, float z)
		{
			makeIdentity();

			_41 = x;
			_42 = y;
			_43 = z;
		}

		void makeScale(float sx, float sy, float sz)
		{
			makeZero();

			_11 = sx; 
			_22 = sy;
			_33 = sz;
			_44 = 1.0f;
		}

		void makeRotationX(float theta)
		{
			float c = cosf(theta);
			float s = sinf(theta);

			_22 = c;
			_23 = s;
			_32 = -s;
			_33 = c;
			_11 = _44 = 1.0f;
			_12 = _13 = _14 = _21 = _24 = _31 = _34 = _41 = _42 = _43 = 0.0f;
		}

		void makeRotationY(float theta)
		{
			float c = cos(theta);
			float s = sin(theta);

			_11 = c;
			_13 = -s;
			_31 = s;
			_33 = c;
			_22 = _44 = 1.0f;
			_12 = _23 = _14 = _21 = _24 = _32 = _34 = _41 = _42 = _43 = 0.0f;
		}

		void makeRotationZ(float theta)
		{
			float c = cos(theta);
			float s = sin(theta);

			_11 = c;
			_12 = s;
			_21 = -s;
			_22 = c;
			_33 = _44 = 1.0f;
			_13 = _14 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
		}

		void makeRotationAxis(const I4Vector3& inAxis, float angle)
		{
			I4Vector3 axis = inAxis;
			axis.normalize();

			float c = cos(angle);
			float s = sin(angle);

			float t = 1.0f - c;

			float xs = axis.x*s;
			float ys = axis.y*s;
			float zs = axis.z*s;

			float xxt = axis.x * axis.x * t;
			float xyt = axis.x * axis.y * t;
			float xzt = axis.x * axis.z * t;
			float yyt = axis.y * axis.y * t;
			float yzt = axis.y * axis.z * t;
			float zzt = axis.z * axis.z * t;

			_11 = xxt + c;			
			_12 = xyt + zs;
			_13 = xzt - ys;
			_21 = xyt - zs;
			_22 = yyt + c;
			_23 = yzt + xs;
			_31 = xzt + ys;
			_32 = yzt - xs;
			_33 = zzt + c;
			
			_14 = _24 = _34 = _41 = _42 = _43 = 0.0f;
			_44 = 1.0f;
		}

		

		void makeOrthoLH(float width, float height, float zNear, float zFar)
		{
			_11 = 2.0f/width;
			_22 = 2.0f/height;
			_33 = 1.0f/(zFar - zNear);
			_43 = -zNear/(zFar - zNear);
			_44 = 1.0f;
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _34 = _41 = _42 = 0.0f;
		}

		void makePerspectiveFovLH(float fovY, float aspect, float zNear, float zFar)
		{
			float yScale = 1.0f/tan(fovY*0.5f);
			float xScale = yScale/aspect;

			_11 = xScale;
			_22 = yScale;
			_33 = zFar/(zFar - zNear);
			_34 = 1.0f;
			_43 = -zNear*zFar/(zFar - zNear);
			_12 = _13 = _14 = _21 = _23 = _24 = _31 = _32 = _41 = _42 = _44 = 0.0f;
		}

		void makeObjectLookAtLH(const I4Vector3& eye, const I4Vector3& lookAt, const I4Vector3& up)
		{
			I4Vector3 zAxis = lookAt - eye;
			zAxis.normalize();

			I4Vector3 xAxis = I4Vector3::crossProduct(up, zAxis);
			xAxis.normalize();

			I4Vector3 yAxis = I4Vector3::crossProduct(zAxis, xAxis);
			yAxis.normalize();
			
			_11 = xAxis.x;	_12 = xAxis.y;	_13 = xAxis.z;
			_21 = yAxis.x;	_22 = yAxis.y;	_23 = yAxis.z;
			_31 = zAxis.x;	_32 = zAxis.y;	_33 = zAxis.z;
			_41 = eye.x;	_42 = eye.y;	_43 = eye.z;
			
			_14 = _24 = _34 = 0.0f;
			_44 = 1.0f;
		}

		void makeCameraLookAtLH(const I4Vector3& eye, const I4Vector3& lookAt, const I4Vector3& up)
		{
			I4Vector3 zAxis = lookAt - eye;
			zAxis.normalize();

			I4Vector3 xAxis = I4Vector3::crossProduct(up, zAxis);
			xAxis.normalize();

			I4Vector3 yAxis = I4Vector3::crossProduct(zAxis, xAxis);
			yAxis.normalize();
			
			_11 = xAxis.x; _12 = yAxis.x; _13 = zAxis.x;
			_21 = xAxis.y; _22 = yAxis.y; _23 = zAxis.y;
			_31 = xAxis.z; _32 = yAxis.z; _33 = zAxis.z;

			_41 = -I4Vector3::dotProduct(xAxis, eye);
			_42 = -I4Vector3::dotProduct(yAxis, eye);
			_43 = -I4Vector3::dotProduct(zAxis, eye);

			_14 = _24 = _34 = 0.0f;
			_44 = 1.0f;
		}

		void decompose(I4Vector3* scale, I4Matrix4x4* rotation, I4Vector3* translation) const
		{
			if (translation != NULL)
			{
				extractTranslation(*translation);
			}

			if (scale != NULL || rotation != NULL)
			{
				I4Vector3 xAxis, yAxis, zAxis;
				extractAxis(xAxis, yAxis, zAxis);

				float xScale = xAxis.getLength();
				float yScale = yAxis.getLength();
				float zScale = zAxis.getLength();

				if (scale != NULL)
				{
					scale->x = xScale;
					scale->y = yScale;
					scale->z = zScale;
				}

				if (rotation != NULL)
				{
					if (xScale != 0)
					{
						float fInvX = 1.0f / xScale;
						xAxis.x *= fInvX;
						xAxis.y *= fInvX;
						xAxis.z *= fInvX;
					}

					if (yScale != 0)
					{
						float fInvY = 1.0f / yScale;
						yAxis.x *= fInvY;
						yAxis.y *= fInvY;
						yAxis.z *= fInvY;
					}

					if (zScale != 0)
					{
						float fInvZ = 1.0f / zScale;
						zAxis.x *= fInvZ;
						zAxis.y *= fInvZ;
						zAxis.z *= fInvZ;
					}

					rotation->_11 = xAxis.x;
					rotation->_12 = xAxis.y;
					rotation->_13 = xAxis.z;

					rotation->_21 = yAxis.x;
					rotation->_22 = yAxis.y;
					rotation->_23 = yAxis.z;

					rotation->_31 = zAxis.x;
					rotation->_32 = zAxis.y;
					rotation->_33 = zAxis.z;

					rotation->_14 = rotation->_24 = rotation->_34 = rotation->_41 = rotation->_42 = rotation->_43 = 0.0f;
					rotation->_44 = 1.0f;
				}
			}			
		}


		const I4Vector3 transformCoord(const I4Vector3& v) const
		{
			return I4Vector3(v.x*_11 + v.y*_21 + v.z*_31 + _41,
							v.x*_12 + v.y*_22 + v.z*_32 + _42,
							v.x*_13 + v.y*_23 + v.z*_33 + _43);
		}

		const I4Vector3 transformCoordTranspose(const I4Vector3& v) const
		{
			return I4Vector3(v.x*_11 + v.y*_12 + v.z*_13 + _14,
						   v.x*_21 + v.y*_22 + v.z*_23 + _24,
						   v.x*_31 + v.y*_32 + v.z*_33 + _34);
		}

		const I4Vector3 transformVector(const I4Vector3& v) const
		{
			return I4Vector3(v.x*_11 + v.y*_21 + v.z*_31,
						   v.x*_12 + v.y*_22 + v.z*_32,
						   v.x*_13 + v.y*_23 + v.z*_33);
		}

		const I4Vector3 transformVectorTranspose(const I4Vector3& v) const
		{
			return I4Vector3(v.x*_11 + v.y*_12 + v.z*_13,
						   v.x*_21 + v.y*_22 + v.z*_23,
						   v.x*_31 + v.y*_32 + v.z*_33);
		}
		
		

	public:
		static void multiply(I4Matrix4x4& result, const I4Matrix4x4& a, const I4Matrix4x4& b)
		{
			assert(&result != &a);
			assert(&result != &b);

			result._11 = a._11*b._11 + a._12*b._21 + a._13*b._31 + a._14*b._41;
			result._12 = a._11*b._12 + a._12*b._22 + a._13*b._32 + a._14*b._42;
			result._13 = a._11*b._13 + a._12*b._23 + a._13*b._33 + a._14*b._43;
			result._14 = a._11*b._14 + a._12*b._24 + a._13*b._34 + a._14*b._44;

			result._21 = a._21*b._11 + a._22*b._21 + a._23*b._31 + a._24*b._41;
			result._22 = a._21*b._12 + a._22*b._22 + a._23*b._32 + a._24*b._42;
			result._23 = a._21*b._13 + a._22*b._23 + a._23*b._33 + a._24*b._43;
			result._24 = a._21*b._14 + a._22*b._24 + a._23*b._34 + a._24*b._44;

			result._31 = a._31*b._11 + a._32*b._21 + a._33*b._31 + a._34*b._41;
			result._32 = a._31*b._12 + a._32*b._22 + a._33*b._32 + a._34*b._42;
			result._33 = a._31*b._13 + a._32*b._23 + a._33*b._33 + a._34*b._43;
			result._34 = a._31*b._14 + a._32*b._24 + a._33*b._34 + a._34*b._44;

			result._41 = a._41*b._11 + a._42*b._21 + a._43*b._31 + a._44*b._41;
			result._42 = a._41*b._12 + a._42*b._22 + a._43*b._32 + a._44*b._42;
			result._43 = a._41*b._13 + a._42*b._23 + a._43*b._33 + a._44*b._43;
			result._44 = a._41*b._14 + a._42*b._24 + a._43*b._34 + a._44*b._44;
		}
		
	public:
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
			float arr[16];
		};
	};


	inline
	const I4Matrix4x4 operator * (float val, const I4Matrix4x4& rhs)
	{
		return I4Matrix4x4(rhs._11*val, rhs._12*val, rhs._13*val, rhs._14*val,
					   rhs._21*val, rhs._22*val, rhs._23*val, rhs._24*val,
					   rhs._31*val, rhs._32*val, rhs._33*val, rhs._34*val,
					   rhs._41*val, rhs._42*val, rhs._43*val, rhs._44*val);
	}	

}