#pragma once

namespace i4core {

	class Variant
	{
	private:
		union VariantValue
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
		enum VariantType
		{
			VT_INVALID = 0,
			VT_CHAR,
			VT_UCHAR,
			VT_SHORT,
			VT_USHORT,
			VT_INT,
			VT_UINT,
			VT_LONG,
			VT_ULONG,
			VT_FLOAT,
			VT_STRING,
			VT_VOID_PTR,
		};
	public:
		Variant()
		{
			dataType = VT_INVALID;
			dataValue.asInt = 0;
		}

		~Variant()
		{
			switch (dataType)
			{
			case VT_STRING:
				delete dataValue.asString;
				break;
			}
		}

		Variant(char v)
		{
			dataType = VT_CHAR;
			dataValue.asChar = v;
		}

		Variant(unsigned char v)
		{
			dataType = VT_UCHAR;
			dataValue.asUChar = v;
		}

		Variant(short v)
		{
			dataType = VT_SHORT;
			dataValue.asShort = v;
		}

		Variant(unsigned short v)
		{
			dataType = VT_USHORT;
			dataValue.asUShort = v;
		}

		Variant(int v)
		{
			dataType = VT_INT;
			dataValue.asInt = v;
		}

		Variant(unsigned int v)
		{
			dataType = VT_UINT;
			dataValue.asUInt = v;
		}

		Variant(long v)
		{
			dataType = VT_LONG;
			dataValue.asLong = v;
		}

		Variant(unsigned long v)
		{
			dataType = VT_ULONG;
			dataValue.asULong = v;
		}

		Variant(float v)
		{
			dataType = VT_FLOAT;
			dataValue.asFloat = v;
		}

		Variant(char* v)
		{
			dataType = VT_STRING;
			dataValue.asString = new string(v);
		}

		Variant(void* v)
		{
			dataType = VT_VOID_PTR;
			dataValue.asVoidPtr = v;
		}

		char asChar()
		{
			assert(dataType == VT_CHAR);
			return dataValue.asChar;
		}

		unsigned char asUChar()
		{
			assert(dataType == VT_UCHAR);
			return dataValue.asUChar;
		}

		short asShort()
		{
			assert(dataType == VT_SHORT);
			return dataValue.asShort;
		}

		unsigned short asUShort()
		{
			assert(dataType == VT_USHORT);
			return dataValue.asUShort;
		}

		int asInt()
		{
			assert(dataType == VT_INT);
			return dataValue.asInt;
		}

		unsigned int asUInt()
		{
			assert(dataType == VT_UINT);
			return dataValue.asUInt;
		}

		long asLong()
		{
			assert(dataType == VT_LONG);
			return dataValue.asLong;
		}

		unsigned long asULong()
		{
			assert(dataType == VT_ULONG);
			return dataValue.asULong;
		}

		float asFloat()
		{
			assert(dataType == VT_FLOAT);
			return dataValue.asFloat;
		}

		const char* asString()
		{
			assert(dataType == VT_STRING);
			return dataValue.asString->c_str();
		}

		void* asVoidPtr()
		{
			assert(dataType == VT_VOID_PTR);
			return dataValue.asVoidPtr;
		}

	private:	
		VariantType		dataType;
		VariantValue		dataValue;
	};
}