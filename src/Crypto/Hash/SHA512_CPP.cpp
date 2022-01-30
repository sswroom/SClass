#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"

#define SHASTEP(v1, v2, v3, v4, v5, v6, v7, v8, n) \
	s1 = ROR64(v5, 14) ^ ROR64(v5, 18) ^ ROR64(v5, 41); \
	ch = (v5 & v6) ^ ((~v5) & v7); \
	temp1 = v8 + s1 + ch + K[n] + w[n]; \
	s0 = ROR64(v1, 28) ^ ROR64(v1, 34) ^ ROR64(v1, 39); \
	maj = (v1 & v2) ^ (v1 & v3) ^ (v2 & v3); \
	temp2 = s0 + maj; \
	v4 += temp1; \
	v8 = temp1 + temp2;

extern "C" void SHA512_CalcBlock(UInt64 *Intermediate_Hash, const UInt8 *Message_Block)
{
	UInt64 w[80];
	static const UInt64 K[] =    {
		0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 
		0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe, 
		0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 
		0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 
		0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab, 
		0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 
		0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 
		0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
		0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 
		0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 
		0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 
		0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 
		0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c, 
		0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 
		0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 
		0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817};
	UOSInt t;
	UInt64 s0;
	UInt64 s1;
	UInt64 ch;
	UInt64 maj;
	UInt64 temp1, temp2;
	UInt64 a, b, c, d, e, f, g, h;

	t = 0;
	while (t < 16)
	{
		w[t] = ReadMUInt64(&Message_Block[t * 8]);
		t++;
	}

	while(t < 80)
	{
		s0 = ROR64(w[t - 15], 1) ^ ROR64(w[t - 15], 8) ^ (w[t - 15] >> 7);
		s1 = ROR64(w[t - 2], 19) ^ ROR64(w[t - 2],  61) ^ (w[t - 2] >> 6);
		w[t] = w[t - 16] + s0 + w[t - 7] + s1;
		t++;
	}

	a = Intermediate_Hash[0];
	b = Intermediate_Hash[1];
	c = Intermediate_Hash[2];
	d = Intermediate_Hash[3];
	e = Intermediate_Hash[4];
	f = Intermediate_Hash[5];
	g = Intermediate_Hash[6];
	h = Intermediate_Hash[7];

/*	t = 0;
	while (t < 80)
	{
        s1 = ROR64(e, 14) ^ ROR64(e, 18) ^ ROR64(e, 41);
        ch = (e & f) ^ ((~e) & g);
        temp1 = h + s1 + ch + K[t] + w[t];
        s0 = ROR64(a, 28) ^ ROR64(a, 34) ^ ROR64(a, 39);
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
	SHASTEP(a, b, c, d, e, f, g, h, 0);
	SHASTEP(h, a, b, c, d, e, f, g, 1);
	SHASTEP(g, h, a, b, c, d, e, f, 2);
	SHASTEP(f, g, h, a, b, c, d, e, 3);
	SHASTEP(e, f, g, h, a, b, c, d, 4);
	SHASTEP(d, e, f, g, h, a, b, c, 5);
	SHASTEP(c, d, e, f, g, h, a, b, 6);
	SHASTEP(b, c, d, e, f, g, h, a, 7);

	SHASTEP(a, b, c, d, e, f, g, h, 8);
	SHASTEP(h, a, b, c, d, e, f, g, 9);
	SHASTEP(g, h, a, b, c, d, e, f, 10);
	SHASTEP(f, g, h, a, b, c, d, e, 11);
	SHASTEP(e, f, g, h, a, b, c, d, 12);
	SHASTEP(d, e, f, g, h, a, b, c, 13);
	SHASTEP(c, d, e, f, g, h, a, b, 14);
	SHASTEP(b, c, d, e, f, g, h, a, 15);

	SHASTEP(a, b, c, d, e, f, g, h, 16);
	SHASTEP(h, a, b, c, d, e, f, g, 17);
	SHASTEP(g, h, a, b, c, d, e, f, 18);
	SHASTEP(f, g, h, a, b, c, d, e, 19);
	SHASTEP(e, f, g, h, a, b, c, d, 20);
	SHASTEP(d, e, f, g, h, a, b, c, 21);
	SHASTEP(c, d, e, f, g, h, a, b, 22);
	SHASTEP(b, c, d, e, f, g, h, a, 23);

	SHASTEP(a, b, c, d, e, f, g, h, 24);
	SHASTEP(h, a, b, c, d, e, f, g, 25);
	SHASTEP(g, h, a, b, c, d, e, f, 26);
	SHASTEP(f, g, h, a, b, c, d, e, 27);
	SHASTEP(e, f, g, h, a, b, c, d, 28);
	SHASTEP(d, e, f, g, h, a, b, c, 29);
	SHASTEP(c, d, e, f, g, h, a, b, 30);
	SHASTEP(b, c, d, e, f, g, h, a, 31);

	SHASTEP(a, b, c, d, e, f, g, h, 32);
	SHASTEP(h, a, b, c, d, e, f, g, 33);
	SHASTEP(g, h, a, b, c, d, e, f, 34);
	SHASTEP(f, g, h, a, b, c, d, e, 35);
	SHASTEP(e, f, g, h, a, b, c, d, 36);
	SHASTEP(d, e, f, g, h, a, b, c, 37);
	SHASTEP(c, d, e, f, g, h, a, b, 38);
	SHASTEP(b, c, d, e, f, g, h, a, 39);

	SHASTEP(a, b, c, d, e, f, g, h, 40);
	SHASTEP(h, a, b, c, d, e, f, g, 41);
	SHASTEP(g, h, a, b, c, d, e, f, 42);
	SHASTEP(f, g, h, a, b, c, d, e, 43);
	SHASTEP(e, f, g, h, a, b, c, d, 44);
	SHASTEP(d, e, f, g, h, a, b, c, 45);
	SHASTEP(c, d, e, f, g, h, a, b, 46);
	SHASTEP(b, c, d, e, f, g, h, a, 47);

	SHASTEP(a, b, c, d, e, f, g, h, 48);
	SHASTEP(h, a, b, c, d, e, f, g, 49);
	SHASTEP(g, h, a, b, c, d, e, f, 50);
	SHASTEP(f, g, h, a, b, c, d, e, 51);
	SHASTEP(e, f, g, h, a, b, c, d, 52);
	SHASTEP(d, e, f, g, h, a, b, c, 53);
	SHASTEP(c, d, e, f, g, h, a, b, 54);
	SHASTEP(b, c, d, e, f, g, h, a, 55);

	SHASTEP(a, b, c, d, e, f, g, h, 56);
	SHASTEP(h, a, b, c, d, e, f, g, 57);
	SHASTEP(g, h, a, b, c, d, e, f, 58);
	SHASTEP(f, g, h, a, b, c, d, e, 59);
	SHASTEP(e, f, g, h, a, b, c, d, 60);
	SHASTEP(d, e, f, g, h, a, b, c, 61);
	SHASTEP(c, d, e, f, g, h, a, b, 62);
	SHASTEP(b, c, d, e, f, g, h, a, 63);

	SHASTEP(a, b, c, d, e, f, g, h, 64);
	SHASTEP(h, a, b, c, d, e, f, g, 65);
	SHASTEP(g, h, a, b, c, d, e, f, 66);
	SHASTEP(f, g, h, a, b, c, d, e, 67);
	SHASTEP(e, f, g, h, a, b, c, d, 68);
	SHASTEP(d, e, f, g, h, a, b, c, 69);
	SHASTEP(c, d, e, f, g, h, a, b, 70);
	SHASTEP(b, c, d, e, f, g, h, a, 71);

	SHASTEP(a, b, c, d, e, f, g, h, 72);
	SHASTEP(h, a, b, c, d, e, f, g, 73);
	SHASTEP(g, h, a, b, c, d, e, f, 74);
	SHASTEP(f, g, h, a, b, c, d, e, 75);
	SHASTEP(e, f, g, h, a, b, c, d, 76);
	SHASTEP(d, e, f, g, h, a, b, c, 77);
	SHASTEP(c, d, e, f, g, h, a, b, 78);
	SHASTEP(b, c, d, e, f, g, h, a, 79);

	Intermediate_Hash[0] += a;
	Intermediate_Hash[1] += b;
	Intermediate_Hash[2] += c;
	Intermediate_Hash[3] += d;
	Intermediate_Hash[4] += e;
	Intermediate_Hash[5] += f;
	Intermediate_Hash[6] += g;
	Intermediate_Hash[7] += h;
}
