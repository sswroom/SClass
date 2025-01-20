#include "Stdafx.h"
#include "Crypto/Encrypt/Encryption.h"
#include "Crypto/Encrypt/QuotedPrintable.h"

extern Char STRHEXARR[];

Crypto::Encrypt::QuotedPrintable::QuotedPrintable()
{
}

Crypto::Encrypt::QuotedPrintable::~QuotedPrintable()
{
}

Int32 Crypto::Encrypt::QuotedPrintable::Encrypt(UnsafeArray<const UInt8> inBuff, Int32 inSize, UnsafeArray<UInt8> outBuff)
{
	Int32 retCnt = 0;
	Int32 lineCnt = 0;
	while (inSize > 0)
	{
		UInt8 b = *inBuff++;
		if (b == '=')
		{
			if (lineCnt > 73)
			{
				*outBuff++ = '=';
				*outBuff++ = '\r';
				*outBuff++ = '\n';
				retCnt += 3;
				lineCnt = 0;
			}
			*outBuff++ = '=';
			*outBuff++ = '3';
			*outBuff++ = 'D';
			retCnt += 3;
			lineCnt += 3;
		}
		else if (b >= 33 && b <= 126)
		{
			if (lineCnt >= 76)
			{
				*outBuff++ = '=';
				*outBuff++ = '\r';
				*outBuff++ = '\n';
				retCnt += 3;
				lineCnt = 0;
			}
			*outBuff++ = b;
			retCnt += 1;
			lineCnt += 1;
		}
		else if (b == 32 || b == 9)
		{
			if (inBuff[0] == 13 && inBuff[1] == 10)
			{
				if (lineCnt > 73)
				{
					*outBuff++ = '=';
					*outBuff++ = '\r';
					*outBuff++ = '\n';
					retCnt += 3;
					lineCnt = 0;
				}
				*outBuff++ = '=';
				*outBuff++ = STRHEXARR[b >> 4];
				*outBuff++ = STRHEXARR[b & 0xf];
				retCnt += 3;
				lineCnt += 3;
			}
			else
			{
				if (lineCnt >= 76)
				{
					*outBuff++ = '=';
					*outBuff++ = '\r';
					*outBuff++ = '\n';
					retCnt += 3;
					lineCnt = 0;
				}
				*outBuff++ = b;
				retCnt++;
				lineCnt++;
			}
		}
		else if (b == 13 && inBuff[0] == 10)
		{
			*outBuff++ = b;
			*outBuff++ = *inBuff++;
			retCnt += 2;
			lineCnt = 0;
			inSize -= 1;
		}
		else
		{
			*outBuff++ = '=';
			*outBuff++ = STRHEXARR[b >> 4];
			*outBuff++ = STRHEXARR[b & 0xf];
			retCnt += 3;
			lineCnt += 3;
		}
	}

	return retCnt;
}

Int32 Crypto::Encrypt::QuotedPrintable::Decrypt(UnsafeArray<const UInt8> inBuff, Int32 inSize, UnsafeArray<UInt8> outBuff)
{
	Int32 retCnt;
	UInt8 b;
	UInt8 b2;
	while (inSize-- > 0)
	{
		b = *inBuff++;
		if (b == '=' && inSize >= 2)
		{
			if (inBuff[0] == 13 && inBuff[1] == 10)
			{
				inSize -= 2;
				inBuff += 2;
			}
			else
			{
				b = *inBuff++;
				if (b >= '0' && b <= '9')
				{
					b2 = b - 0x30;
				}
				else if (b >= 'A' && b <= 'Z')
				{
					b2 = b - 0x37;
				}
				else if (b >= 'a' && b <= 'z')
				{
					b2 = b - 0x57;
				}
				else
				{
					b2 = 0;
				}
				b = *inBuff++;
				if (b >= '0' && b <= '9')
				{
					*outBuff++ = (b2 << 4) + (b - 0x30);
				}
				else if (b >= 'A' && b <= 'Z')
				{
					*outBuff++ = (b2 << 4) + (b - 0x37);
				}
				else if (b >= 'a' && b <= 'z')
				{
					*outBuff++ = (b2 << 4) + (b - 0x57);
				}
				else
				{
					*outBuff++ = (b2 << 4);
				}
				retCnt++;
			}
		}
		else
		{
			*outBuff++ = b;
			retCnt++;
		}
	}
	return retCnt;
}
