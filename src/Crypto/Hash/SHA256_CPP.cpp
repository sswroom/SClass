#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"

#define SHASTEP1(v1, v2, v3, v4, v5, v6, v7, v8, n, k) \
	temp1 = w[n] = ReadMUInt32(&Message_Block[n * 4]); \
	temp1 += ROR32(v5, 6) ^ ROR32(v5, 11) ^ ROR32(v5, 25); \
	temp1 += (v5 & v6) ^ ((~v5) & v7); \
	temp1 += v8 + k; \
	temp2 = ROR32(v1, 2) ^ ROR32(v1, 13) ^ ROR32(v1, 22); \
	temp2 += (v1 & v2) ^ (v1 & v3) ^ (v2 & v3); \
	v4 += temp1; \
	v8 = temp1 + temp2;

#define SHASTEP2(v1, v2, v3, v4, v5, v6, v7, v8, n, k) \
	temp1 = w[n] = w[n - 16] + (ROR32(w[n - 15], 7) ^ ROR32(w[n - 15], 18) ^ (w[n - 15] >> 3)) + w[n - 7] + (ROR32(w[n - 2], 17) ^ ROR32(w[n - 2],  19) ^ (w[n - 2] >> 10)); \
	temp1 += ROR32(v5, 6) ^ ROR32(v5, 11) ^ ROR32(v5, 25); \
	temp1 += (v5 & v6) ^ ((~v5) & v7); \
	temp1 += v8 + k; \
	temp2 = ROR32(v1, 2) ^ ROR32(v1, 13) ^ ROR32(v1, 22); \
	temp2 += (v1 & v2) ^ (v1 & v3) ^ (v2 & v3); \
	v4 += temp1; \
	v8 = temp1 + temp2;

extern "C" void SHA256_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
{
	UInt32 w[64];
	UInt32 temp1, temp2;
	UInt32 a, b, c, d, e, f, g, h;

/*	t = 0;
	while (t < 16)
	{
		w[t] = ReadMUInt32(&Message_Block[t * 4]);
		t++;
	}

	while(t < 64)
	{
		s0 = ROR32(w[t - 15], 7) ^ ROR32(w[t - 15], 18) ^ (w[t - 15] >> 3);
		s1 = ROR32(w[t - 2], 17) ^ ROR32(w[t - 2],  19) ^ (w[t - 2] >> 10);
		w[t] = w[t - 16] + s0 + w[t - 7] + s1;
		t++;
	}*/

	a = Intermediate_Hash[0];
	b = Intermediate_Hash[1];
	c = Intermediate_Hash[2];
	d = Intermediate_Hash[3];
	e = Intermediate_Hash[4];
	f = Intermediate_Hash[5];
	g = Intermediate_Hash[6];
	h = Intermediate_Hash[7];

/*	t = 0;
	while (t < 64)
	{
        s1 = ROR32(e, 6) ^ ROR32(e, 11) ^ ROR32(e, 25);
        ch = (e & f) ^ ((~e) & g);
        temp1 = h + s1 + ch + K[t] + w[t];
        s0 = ROR32(a, 2) ^ ROR32(a, 13) ^ ROR32(a, 22);
        maj = (a & b) ^ (a & c) ^ (b & c);
        temp2 = s0 + maj;

		h = g;
		g = f;
		f = e;
		e = d + temp1;
		d = c;
		c = b;
		b = a;
        a = temp1 + temp2;

		t++;
    }*/
	SHASTEP1(a, b, c, d, e, f, g, h, 0, 0x428a2f98);
	SHASTEP1(h, a, b, c, d, e, f, g, 1, 0x71374491);
	SHASTEP1(g, h, a, b, c, d, e, f, 2, 0xb5c0fbcf);
	SHASTEP1(f, g, h, a, b, c, d, e, 3, 0xe9b5dba5);
	SHASTEP1(e, f, g, h, a, b, c, d, 4, 0x3956c25b);
	SHASTEP1(d, e, f, g, h, a, b, c, 5, 0x59f111f1);
	SHASTEP1(c, d, e, f, g, h, a, b, 6, 0x923f82a4);
	SHASTEP1(b, c, d, e, f, g, h, a, 7, 0xab1c5ed5);

	SHASTEP1(a, b, c, d, e, f, g, h, 8, 0xd807aa98);
	SHASTEP1(h, a, b, c, d, e, f, g, 9, 0x12835b01);
	SHASTEP1(g, h, a, b, c, d, e, f, 10, 0x243185be);
	SHASTEP1(f, g, h, a, b, c, d, e, 11, 0x550c7dc3);
	SHASTEP1(e, f, g, h, a, b, c, d, 12, 0x72be5d74);
	SHASTEP1(d, e, f, g, h, a, b, c, 13, 0x80deb1fe);
	SHASTEP1(c, d, e, f, g, h, a, b, 14, 0x9bdc06a7);
	SHASTEP1(b, c, d, e, f, g, h, a, 15, 0xc19bf174);

	SHASTEP2(a, b, c, d, e, f, g, h, 16, 0xe49b69c1);
	SHASTEP2(h, a, b, c, d, e, f, g, 17, 0xefbe4786);
	SHASTEP2(g, h, a, b, c, d, e, f, 18, 0x0fc19dc6);
	SHASTEP2(f, g, h, a, b, c, d, e, 19, 0x240ca1cc);
	SHASTEP2(e, f, g, h, a, b, c, d, 20, 0x2de92c6f);
	SHASTEP2(d, e, f, g, h, a, b, c, 21, 0x4a7484aa);
	SHASTEP2(c, d, e, f, g, h, a, b, 22, 0x5cb0a9dc);
	SHASTEP2(b, c, d, e, f, g, h, a, 23, 0x76f988da);

	SHASTEP2(a, b, c, d, e, f, g, h, 24, 0x983e5152);
	SHASTEP2(h, a, b, c, d, e, f, g, 25, 0xa831c66d);
	SHASTEP2(g, h, a, b, c, d, e, f, 26, 0xb00327c8);
	SHASTEP2(f, g, h, a, b, c, d, e, 27, 0xbf597fc7);
	SHASTEP2(e, f, g, h, a, b, c, d, 28, 0xc6e00bf3);
	SHASTEP2(d, e, f, g, h, a, b, c, 29, 0xd5a79147);
	SHASTEP2(c, d, e, f, g, h, a, b, 30, 0x06ca6351);
	SHASTEP2(b, c, d, e, f, g, h, a, 31, 0x14292967);

	SHASTEP2(a, b, c, d, e, f, g, h, 32, 0x27b70a85);
	SHASTEP2(h, a, b, c, d, e, f, g, 33, 0x2e1b2138);
	SHASTEP2(g, h, a, b, c, d, e, f, 34, 0x4d2c6dfc);
	SHASTEP2(f, g, h, a, b, c, d, e, 35, 0x53380d13);
	SHASTEP2(e, f, g, h, a, b, c, d, 36, 0x650a7354);
	SHASTEP2(d, e, f, g, h, a, b, c, 37, 0x766a0abb);
	SHASTEP2(c, d, e, f, g, h, a, b, 38, 0x81c2c92e);
	SHASTEP2(b, c, d, e, f, g, h, a, 39, 0x92722c85);

	SHASTEP2(a, b, c, d, e, f, g, h, 40, 0xa2bfe8a1);
	SHASTEP2(h, a, b, c, d, e, f, g, 41, 0xa81a664b);
	SHASTEP2(g, h, a, b, c, d, e, f, 42, 0xc24b8b70);
	SHASTEP2(f, g, h, a, b, c, d, e, 43, 0xc76c51a3);
	SHASTEP2(e, f, g, h, a, b, c, d, 44, 0xd192e819);
	SHASTEP2(d, e, f, g, h, a, b, c, 45, 0xd6990624);
	SHASTEP2(c, d, e, f, g, h, a, b, 46, 0xf40e3585);
	SHASTEP2(b, c, d, e, f, g, h, a, 47, 0x106aa070);

	SHASTEP2(a, b, c, d, e, f, g, h, 48, 0x19a4c116);
	SHASTEP2(h, a, b, c, d, e, f, g, 49, 0x1e376c08);
	SHASTEP2(g, h, a, b, c, d, e, f, 50, 0x2748774c);
	SHASTEP2(f, g, h, a, b, c, d, e, 51, 0x34b0bcb5);
	SHASTEP2(e, f, g, h, a, b, c, d, 52, 0x391c0cb3);
	SHASTEP2(d, e, f, g, h, a, b, c, 53, 0x4ed8aa4a);
	SHASTEP2(c, d, e, f, g, h, a, b, 54, 0x5b9cca4f);
	SHASTEP2(b, c, d, e, f, g, h, a, 55, 0x682e6ff3);

	SHASTEP2(a, b, c, d, e, f, g, h, 56, 0x748f82ee);
	SHASTEP2(h, a, b, c, d, e, f, g, 57, 0x78a5636f);
	SHASTEP2(g, h, a, b, c, d, e, f, 58, 0x84c87814);
	SHASTEP2(f, g, h, a, b, c, d, e, 59, 0x8cc70208);
	SHASTEP2(e, f, g, h, a, b, c, d, 60, 0x90befffa);
	SHASTEP2(d, e, f, g, h, a, b, c, 61, 0xa4506ceb);
	SHASTEP2(c, d, e, f, g, h, a, b, 62, 0xbef9a3f7);
	SHASTEP2(b, c, d, e, f, g, h, a, 63, 0xc67178f2);

	Intermediate_Hash[0] += a;
	Intermediate_Hash[1] += b;
	Intermediate_Hash[2] += c;
	Intermediate_Hash[3] += d;
	Intermediate_Hash[4] += e;
	Intermediate_Hash[5] += f;
	Intermediate_Hash[6] += g;
	Intermediate_Hash[7] += h;
}
