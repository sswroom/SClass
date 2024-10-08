#include "Stdafx.h"

#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (z ^ y ^ x)
#define F4(x, y, z) (y ^ (x | ~z))
#define MD5STEP(f, w, x, y, z, data, s) (w += data + f(x, y, z), w = w<<s | w>>(32-s),  w += x)

extern "C" void MD5_CalcBlock(UInt32 *hVals, const UInt8 *block, UOSInt blockCnt)
{
	UInt32 *blk = (UInt32*)block;
	UInt32 a = hVals[0];
	UInt32 b = hVals[1];
	UInt32 c = hVals[2];
	UInt32 d = hVals[3];

	while (blockCnt-- > 0)
	{
		MD5STEP(F1, a, b, c, d, blk[0] + 0xd76aa478, 7);
		MD5STEP(F1, d, a, b, c, blk[1] + 0xe8c7b756, 12);
		MD5STEP(F1, c, d, a, b, blk[2] + 0x242070db, 17);
		MD5STEP(F1, b, c, d, a, blk[3] + 0xc1bdceee, 22);
		MD5STEP(F1, a, b, c, d, blk[4] + 0xf57c0faf, 7);
		MD5STEP(F1, d, a, b, c, blk[5] + 0x4787c62a, 12);
		MD5STEP(F1, c, d, a, b, blk[6] + 0xa8304613, 17);
		MD5STEP(F1, b, c, d, a, blk[7] + 0xfd469501, 22);
		MD5STEP(F1, a, b, c, d, blk[8] + 0x698098d8, 7);
		MD5STEP(F1, d, a, b, c, blk[9] + 0x8b44f7af, 12);
		MD5STEP(F1, c, d, a, b, blk[10] + 0xffff5bb1, 17);
		MD5STEP(F1, b, c, d, a, blk[11] + 0x895cd7be, 22);
		MD5STEP(F1, a, b, c, d, blk[12] + 0x6b901122, 7);
		MD5STEP(F1, d, a, b, c, blk[13] + 0xfd987193, 12);
		MD5STEP(F1, c, d, a, b, blk[14] + 0xa679438e, 17);
		MD5STEP(F1, b, c, d, a, blk[15] + 0x49b40821, 22);

		MD5STEP(F2, a, b, c, d, blk[1] + 0xf61e2562, 5);
		MD5STEP(F2, d, a, b, c, blk[6] + 0xc040b340, 9);
		MD5STEP(F2, c, d, a, b, blk[11] + 0x265e5a51, 14);
		MD5STEP(F2, b, c, d, a, blk[0] + 0xe9b6c7aa, 20);
		MD5STEP(F2, a, b, c, d, blk[5] + 0xd62f105d, 5);
		MD5STEP(F2, d, a, b, c, blk[10] + 0x02441453, 9);
		MD5STEP(F2, c, d, a, b, blk[15] + 0xd8a1e681, 14);
		MD5STEP(F2, b, c, d, a, blk[4] + 0xe7d3fbc8, 20);
		MD5STEP(F2, a, b, c, d, blk[9] + 0x21e1cde6, 5);
		MD5STEP(F2, d, a, b, c, blk[14] + 0xc33707d6, 9);
		MD5STEP(F2, c, d, a, b, blk[3] + 0xf4d50d87, 14);
		MD5STEP(F2, b, c, d, a, blk[8] + 0x455a14ed, 20);
		MD5STEP(F2, a, b, c, d, blk[13] + 0xa9e3e905, 5);
		MD5STEP(F2, d, a, b, c, blk[2] + 0xfcefa3f8, 9);
		MD5STEP(F2, c, d, a, b, blk[7] + 0x676f02d9, 14);
		MD5STEP(F2, b, c, d, a, blk[12] + 0x8d2a4c8a, 20);

		MD5STEP(F3, a, b, c, d, blk[5] + 0xfffa3942, 4);
		MD5STEP(F3, d, a, b, c, blk[8] + 0x8771f681, 11);
		MD5STEP(F3, c, d, a, b, blk[11] + 0x6d9d6122, 16);
		MD5STEP(F3, b, c, d, a, blk[14] + 0xfde5380c, 23);
		MD5STEP(F3, a, b, c, d, blk[1] + 0xa4beea44, 4);
		MD5STEP(F3, d, a, b, c, blk[4] + 0x4bdecfa9, 11);
		MD5STEP(F3, c, d, a, b, blk[7] + 0xf6bb4b60, 16);
		MD5STEP(F3, b, c, d, a, blk[10] + 0xbebfbc70, 23);
		MD5STEP(F3, a, b, c, d, blk[13] + 0x289b7ec6, 4);
		MD5STEP(F3, d, a, b, c, blk[0] + 0xeaa127fa, 11);
		MD5STEP(F3, c, d, a, b, blk[3] + 0xd4ef3085, 16);
		MD5STEP(F3, b, c, d, a, blk[6] + 0x04881d05, 23);
		MD5STEP(F3, a, b, c, d, blk[9] + 0xd9d4d039, 4);
		MD5STEP(F3, d, a, b, c, blk[12] + 0xe6db99e5, 11);
		MD5STEP(F3, c, d, a, b, blk[15] + 0x1fa27cf8, 16);
		MD5STEP(F3, b, c, d, a, blk[2] + 0xc4ac5665, 23);

		MD5STEP(F4, a, b, c, d, blk[0] + 0xf4292244, 6);
		MD5STEP(F4, d, a, b, c, blk[7] + 0x432aff97, 10);
		MD5STEP(F4, c, d, a, b, blk[14] + 0xab9423a7, 15);
		MD5STEP(F4, b, c, d, a, blk[5] + 0xfc93a039, 21);
		MD5STEP(F4, a, b, c, d, blk[12] + 0x655b59c3, 6);
		MD5STEP(F4, d, a, b, c, blk[3] + 0x8f0ccc92, 10);
		MD5STEP(F4, c, d, a, b, blk[10] + 0xffeff47d, 15);
		MD5STEP(F4, b, c, d, a, blk[1] + 0x85845dd1, 21);
		MD5STEP(F4, a, b, c, d, blk[8] + 0x6fa87e4f, 6);
		MD5STEP(F4, d, a, b, c, blk[15] + 0xfe2ce6e0, 10);
		MD5STEP(F4, c, d, a, b, blk[6] + 0xa3014314, 15);
		MD5STEP(F4, b, c, d, a, blk[13] + 0x4e0811a1, 21);
		MD5STEP(F4, a, b, c, d, blk[4] + 0xf7537e82, 6);
		MD5STEP(F4, d, a, b, c, blk[11] + 0xbd3af235, 10);
		MD5STEP(F4, c, d, a, b, blk[2] + 0x2ad7d2bb, 15);
		MD5STEP(F4, b, c, d, a, blk[9] + 0xeb86d391, 21);

		a += hVals[0];
		b += hVals[1];
		c += hVals[2];
		d += hVals[3];
		hVals[0] = a;
		hVals[1] = b;
		hVals[2] = c;
		hVals[3] = d;
		blk += 16;
	}
}
