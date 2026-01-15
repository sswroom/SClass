#include "Stdafx.h"
#include "Crypto/Encrypt/FormEncode.h"

extern Char STRHEXARR[];

UInt8 Crypto::Encrypt::FormEncode::URIAllow[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

Crypto::Encrypt::FormEncode::FormEncode()
{
}

Crypto::Encrypt::FormEncode::~FormEncode()
{
}

UIntOS Crypto::Encrypt::FormEncode::Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 c;
	UnsafeArray<UInt8> dest = outBuff;
	while (inSize-- > 0)
	{
		c = *inBuff++;
		if (URIAllow[c])
		{
			*dest++ = c;
		}
		else
		{
			if (c == ' ')
			{
				*dest++ = '+';
			}
			else
			{
				*dest++ = '%';
				*dest++ = (UInt8)STRHEXARR[c >> 4];
				*dest++ = (UInt8)STRHEXARR[c & 15];
			}
		}
	}
	return (UIntOS)(dest - outBuff);
}

UIntOS Crypto::Encrypt::FormEncode::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UnsafeArray<UInt8> dest = outBuff;
	UInt8 c;
	UInt8 v;
	while (inSize > 0)
	{
		c = *inBuff++;
		if (c == '+')
		{
			*dest++ = ' ';
			inSize--;
		}
		else if (c == '%' && inSize >= 3)
		{
			c = *inBuff++;
			if (c >= 0x30 && c <= 0x39)
			{
				v = c - 0x30;
			}
			else if (c >= 0x41 && c <= 0x46)
			{
				v = c - 0x37;
			}
			else if (c >= 0x61 && c <= 0x66)
			{
				v = c - 0x57;
			}
			else
			{
				*dest++ = '%';
				*dest++ = (UInt8)c;
				c = 0;
				inSize -= 2;
			}
			if (c)
			{
				c = *inBuff++;
				if (c >= 0x30 && c <= 0x39)
				{
					*dest++ = (v << 4) + (c - 0x30);
				}
				else if (c >= 0x41 && c <= 0x46)
				{
					*dest++ = (v << 4) + (c - 0x37);
				}
				else if (c >= 0x61 && c <= 0x66)
				{
					*dest++ = (v << 4) + (c - 0x57);
				}
				else
				{
					*dest++ = '%';
					*dest++ = (UInt8)inBuff[-2];
					*dest++ = (UInt8)c;
				}
				inSize -= 3;
			}
		}
		else
		{
			*dest++ = c;
			inSize--;
		}
	}
	return (UIntOS)(dest - outBuff);
}

UIntOS Crypto::Encrypt::FormEncode::GetEncBlockSize() const
{
	return 1;
}

UIntOS Crypto::Encrypt::FormEncode::GetDecBlockSize() const
{
	return 1;
}
