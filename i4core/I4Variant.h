#pragma once

namespace i4core {

	class I4Variant
	{
	private:
		union I4VariantValue
		{
			char			asChar;
			unsigned char	asUChar;
			short			asShort;
			unsigned short	asUShort;
			int				asInt;
			unsigned int	asUInt;
			long			asLong;
			unsigned long	asULong;
			float			asFloat;
			string*			asString;
			void*			asVoidPtr;
		};
	public:
		enum I4VariantType
		{
			I4VT_INVALID = 0,
			I4VT_CHAR,
			I4VT_UCHAR,
			I4VT_SHORT,
			I4VT_USHORT,
			I4VT_INT,
			I4VT_UINT,
			I4VT_LONG,
			I4VT_ULONG,
			I4VT_FLOAT,
			I4VT_STRING,
			I4VT_VOID_PTR,
		};
	public:
		I4Variant()
		{
			dataType = I4VT_INVALID;
			dataValue.asInt = 0;
		}

		~I4Variant()
		{
			switch (dataType)
			{
			case I4VT_STRING:
				delete dataValue.asString;
				break;
			}
		}

		I4Variant(char v)
		{
			dataType = I4VT_CHAR;
			dataValue.asChar = v;
		}

		I4Variant(unsigned char v)
		{
			dataType = I4VT_UCHAR;
			dataValue.asUChar = v;
		}

		I4Variant(short v)
		{
			dataType = I4VT_SHORT;
			dataValue.asShort = v;
		}

		I4Variant(unsigned short v)
		{
			dataType = I4VT_USHORT;
			dataValue.asUShort = v;
		}

		I4Variant(int v)
		{
			dataType = I4VT_INT;
			dataValue.asInt = v;
		}

		I4Variant(unsigned int v)
		{
			dataType = I4VT_UINT;
			dataValue.asUInt = v;
		}

		I4Variant(long v)
		{
			dataType = I4VT_LONG;
			dataValue.asLong = v;
		}

		I4Variant(unsigned long v)
		{
			dataType = I4VT_ULONG;
			dataValue.asULong = v;
		}

		I4Variant(float v)
		{
			dataType = I4VT_FLOAT;
			dataValue.asFloat = v;
		}

		I4Variant(char* v)
		{
			dataType = I4VT_STRING;
			dataValue.asString = new string(v);
		}

		I4Variant(void* v)
		{
			dataType = I4VT_VOID_PTR;
			dataValue.asVoidPtr = v;
		}

		char asChar()
		{
			assert(dataType == I4VT_CHAR);
			return dataValue.asChar;
		}

		unsigned char asUChar()
		{
			assert(dataType == I4VT_UCHAR);
			return dataValue.asUChar;
		}

		short asShort()
		{
			assert(dataType == I4VT_SHORT);
			return dataValue.asShort;
		}

		unsigned short asUShort()
		{
			assert(dataType == I4VT_USHORT);
			return dataValue.asUShort;
		}

		int asInt()
		{
			assert(dataType == I4VT_INT);
			return dataValue.asInt;
		}

		unsigned int asUInt()
		{
			assert(dataType == I4VT_UINT);
			return dataValue.asUInt;
		}

		long asLong()
		{
			assert(dataType == I4VT_LONG);
			return dataValue.asLong;
		}

		unsigned long asULong()
		{
			assert(dataType == I4VT_ULONG);
			return dataValue.asULong;
		}

		float asFloat()
		{
			assert(dataType == I4VT_FLOAT);
			return dataValue.asFloat;
		}

		const char* asString()
		{
			assert(dataType == I4VT_STRING);
			return dataValue.asString->c_str();
		}

		void* asVoidPtr()
		{
			assert(dataType == I4VT_VOID_PTR);
			return dataValue.asVoidPtr;
		}

	private:	
		I4VariantType		dataType;
		I4VariantValue		dataValue;
	};
}