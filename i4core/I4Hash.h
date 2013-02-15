#pragma once

#include "i4core.h"

// GPG 5.1.3 참조
// Fowler /Noll / Vo (FNV) Hash
// http://isthe.com/chongo/tech/comp/fnv/
// 어디까지나 해시니까 충돌가능성이 있다.
// 사용시에는 항상 원본이 맞는지 확인할수 있는 수단이 었어야 한다.
#define I4INVALID_HASHCODE 0xffffffff
#define I4HASH_INIT	0x811c9dc5
#define I4HASH_PRIME	0x01000193

namespace i4core
{
	class I4Hash
	{
	public:
		I4Hash()
			: hashCode(I4INVALID_HASHCODE)
		{
		}

		I4Hash(const string& str)
		{
			hashCode = getHashCode(str.c_str());
		}

		I4Hash(const char* str)
		{
			hashCode = getHashCode(str);
		}

		I4Hash(unsigned int _hashCode)
		{
			hashCode = _hashCode;
		}

		I4Hash(const I4Hash& rhs)
		{
			hashCode = rhs.hashCode;
		}

		const I4Hash& operator = (const I4Hash& rhs)
		{
			hashCode = rhs.hashCode;
			return *this;
		}

		operator unsigned int ()
		{
			return hashCode;
		}

		const bool operator < (const I4Hash &rhs) const
		{
			return hashCode < rhs.hashCode;
		}

		const bool operator > (const I4Hash &rhs) const
		{
			return hashCode > rhs.hashCode;
		}

		const bool operator <= (const I4Hash &rhs) const
		{
			return hashCode <= rhs.hashCode;
		}

		const bool operator >= (const I4Hash &rhs) const
		{
			return hashCode >= rhs.hashCode;
		}

		const bool operator == (const I4Hash &rhs) const
		{
			return hashCode == rhs.hashCode;
		}

		const bool operator == (unsigned int code) const
		{
			return hashCode == code;
		}

		const bool operator != (const I4Hash &rhs) const
		{
			return hashCode != rhs.hashCode;
		}

		const bool operator != (unsigned int code) const
		{
			return hashCode != code;
		}

	public:
		static unsigned int getHashCode(const char* str)
		{
			if (!str || !str[0])
				return I4INVALID_HASHCODE;

			const unsigned char* st = (const unsigned char*)str;
			unsigned int hash = I4HASH_INIT;

			while (*st)
			{
				hash *= I4HASH_PRIME;

				char c = *st++;
				if (c == '\\')
				{
					c = '/';
				}
				else if ((c >= 'a') && (c <= 'z'))
				{
					c -= 'a' - 'A';
				}

				hash ^= (unsigned int)c;
			}
			return hash;
		}

	private:
		unsigned int hashCode;
	};
}

