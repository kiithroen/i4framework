#pragma once

#include "i4core.h"

// GPG 5.1.3 참조
// Fowler /Noll / Vo (FNV) Hash
// http://isthe.com/chongo/tech/comp/fnv/
// 어디까지나 해시니까 충돌가능성이 있다.
// 사용시에는 항상 원본이 맞는지 확인할수 있는 수단이 었어야 한다.
#define INVALID_HASH 0xffffffff
#define HASH_INIT	0x811c9dc5
#define HASH_PRIME	0x01000193

namespace i4core
{
	class I4HashID
	{
	public:
		I4HashID()
			: hashCode(INVALID_HASH)
		{
		}

		I4HashID(const string& str)
		{
			hashCode = getHashCode(str.c_str());
		}

		I4HashID(const char* str)
		{
			hashCode = getHashCode(str);
		}

		I4HashID(unsigned int _hashValue)
		{
			hashCode = _hashValue;
		}

		I4HashID(const I4HashID& rhs)
		{
			hashCode = rhs.hashCode;
		}

		const I4HashID& operator = (const I4HashID& rhs)
		{
			hashCode = rhs.hashCode;
			return *this;
		}

		operator unsigned int ()
		{
			return hashCode;
		}

		const bool operator < (const I4HashID &rhs) const
		{
			return hashCode < rhs.hashCode;
		}

		const bool operator > (const I4HashID &rhs) const
		{
			return hashCode > rhs.hashCode;
		}

		const bool operator <= (const I4HashID &rhs) const
		{
			return hashCode <= rhs.hashCode;
		}

		const bool operator >= (const I4HashID &rhs) const
		{
			return hashCode >= rhs.hashCode;
		}

		const bool operator == (const I4HashID &rhs) const
		{
			return hashCode == rhs.hashCode;
		}

		const bool operator != (const I4HashID &rhs) const
		{
			return hashCode != rhs.hashCode;
		}

	public:
		static unsigned int getHashCode(const char* str)
		{
			if (!str || !str[0])
				return INVALID_HASH;

			const unsigned char* st = (const unsigned char*)str;
			unsigned int hash = HASH_INIT;

			while (*st)
			{
				hash *= HASH_PRIME;

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

	// 대소문자를 구별하지 않는 이름을 기반으로한 해쉬를 아이디로 사용하는 클래스
	// 아이디는 적지만 충돌의 가능성이 있으므로 주의해야한다.
	template <typename T>
	class I4HashData
	{
	public:
		void setName(const string& _name)
		{
			name = _name;
			id = I4HashID(name);
		}

		bool isEqual(const I4HashData<T>& rhs)
		{
			return (sameID(rhs.id) && sameName(name));
		}

		bool sameID(I4HashID _id)
		{
			return id == _id;
		}

		bool sameName(const string& _name)
		{
			return sameName(_name.c_str());
		}

		bool sameName(const char* _name)
		{
			return (_stricmp(name.c_str(), _name) == 0);
		}

		I4HashID			getID()		{ return id; }
		const string&	getName()	{ return name; }

	protected:
		I4HashData()	{}
		~I4HashData()	{}

	private:
		I4HashData(const I4HashData&);
		const I4HashData& operator = (const I4HashData&);

	private:
		I4HashID			id;
		string		name;
	};
}

