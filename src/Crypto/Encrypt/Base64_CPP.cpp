#include "Stdafx.h"
#if (!defined(WIN32) && defined(CPU_X86_32)) || (defined(CPU_X86_64) && !defined(__MACH__) && !defined(WIN32))
#define Base64_encArr _Base64_encArr
#define Base64_decArr _Base64_decArr
#endif
extern "C"
{
	extern Char *Base64_encArr;
	extern UInt8 Base64_decArr[];
	IntOS Base64_Encrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff);
	IntOS Base64_Decrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff);
}
IntOS Base64_Encrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff)
{
	IntOS tmp1 = inSize % 3;
	IntOS tmp2 = inSize / 3;
	IntOS retSize;
	if (tmp1 == 0)
	{
		retSize = tmp2 << 2;
	}
	else
	{
		retSize = (tmp2 + 1) << 2;
	}
	if (retSize == 0)
		return retSize;
	if (outBuff == 0)
		return retSize;

	while (tmp2-- > 0)
	{
		outBuff[0] = (UInt8)Base64_encArr[inBuff[0] >> 2];
		outBuff[1] = (UInt8)Base64_encArr[((inBuff[0] << 4) | (inBuff[1] >> 4)) & 0x3f];
		outBuff[2] = (UInt8)Base64_encArr[((inBuff[1] << 2) | (inBuff[2] >> 6)) & 0x3f];
		outBuff[3] = (UInt8)Base64_encArr[inBuff[2] & 0x3f];
		outBuff += 4;
		inBuff += 3;
	}
	if (tmp1 == 1)
	{
		outBuff[0] = (UInt8)Base64_encArr[inBuff[0] >> 2];
		outBuff[1] = (UInt8)Base64_encArr[(inBuff[0] << 4) & 0x3f];
		outBuff[2] = '=';
		outBuff[3] = '=';
	}
	else if (tmp1 == 2)
	{
		outBuff[0] = (UInt8)Base64_encArr[inBuff[0] >> 2];
		outBuff[1] = (UInt8)Base64_encArr[((inBuff[0] << 4) | (inBuff[1] >> 4)) & 0x3f];
		outBuff[2] = (UInt8)Base64_encArr[(inBuff[1] << 2) & 0x3f];
		outBuff[3] = '=';
	}
	return retSize;
}

IntOS Base64_Decrypt(const UInt8 *inBuff, IntOS inSize, UInt8 *outBuff)
{
	UInt8 *startPos = outBuff;
	UInt8 b;
	UInt8 c = 0;
	if (inSize & 3)
		return 0;

	while (inSize-- > 0)
	{
		b = *inBuff++;
		if ((b = Base64_decArr[b]) == 0xff)
		{
			continue;
		}
		if (c == 0)
		{
			outBuff[0] = (b << 2);
		}
		else if (c == 1)
		{
			outBuff[0] |= b >> 4;
			outBuff += 1;
			outBuff[0] = b << 4;
		}
		else if (c == 2)
		{
			outBuff[0] |= b >> 2;
			outBuff += 1;
			outBuff[0] = b << 6;
		}
		else
		{
			outBuff[0] |= b;
			outBuff += 1;
		}
		c = (c + 1) & 3;
	}
	return outBuff - startPos;
}
