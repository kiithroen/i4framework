#pragma once

// GPG 5.1.3 ����
// Fowler /Noll / Vo (FNV) Hash
// http://isthe.com/chongo/tech/comp/fnv/
// �������� �ؽôϱ� �浹���ɼ��� �ִ�.
// ���ÿ��� �׻� ������ �´��� Ȯ���Ҽ� �ִ� ������ ����� �Ѵ�.
#define INVALID_HASHCODE 0xffffffff
#define HASH_INIT	0x811c9dc5
#define HASH_PRIME	0x01000193

namespace i4core
{
	class Hash
	{
	public:
		Hash()
			: hashCode(INVALID_HASHCODE)
		{
		}

		Hash(const string& str)
		{
			hashCode = getHashCode(str.c_str());
		}

		Hash(const char* str)
		{
			hashCode = getHashCode(str);
		}

		Hash(unsigned int _hashCode)
		{
			hashCode = _hashCode;
		}

		Hash(const Hash& rhs)
		{
			hashCode = rhs.hashCode;
		}

		const Hash& operator = (const Hash& rhs)
		{
			hashCode = rhs.hashCode;
			return *this;
		}

		operator unsigned int ()
		{
			return hashCode;
		}

		const bool operator < (const Hash &rhs) const
		{
			return hashCode < rhs.hashCode;
		}

		const bool operator > (const Hash &rhs) const
		{
			return hashCode > rhs.hashCode;
		}

		const bool operator <= (const Hash &rhs) const
		{
			return hashCode <= rhs.hashCode;
		}

		const bool operator >= (const Hash &rhs) const
		{
			return hashCode >= rhs.hashCode;
		}

		const bool operator == (const Hash &rhs) const
		{
			return hashCode == rhs.hashCode;
		}

		const bool operator == (unsigned int code) const
		{
			return hashCode == code;
		}

		const bool operator != (const Hash &rhs) const
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
				return INVALID_HASHCODE;

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
}

