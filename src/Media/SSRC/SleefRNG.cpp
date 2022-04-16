#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Media/SSRC/SleefRNG.h"

#define C0 0xf3dd0fb7820fde37ULL
#define C1 0xe6c6ac2c59e52811ULL
#define C2 0x2fc7871fff7c5b45ULL
#define C3 0x47c7e1f70aa4f7c5ULL
#define C4 0x094f02b7fb9ba895ULL
#define C5 0x89afda817e744570ULL
#define C6 0xc7277d052c7bf14bULL
#define C7 0x474f4433b10b081dULL
#define C8 0xa0a543d9f16196a5ULL
#define C9 0x06dce455629a8955ULL

UInt64 Media::SSRC::SleefRNG::Xrandom64()
{
	UOSInt n;
	this->state = NextState(this->state);
	UInt64 s = this->state.u ^ this->state.l;
	UInt64 t = s;
	n = (s >> 24) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C0;
	n = (s >>  6) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C1;
	n = (s >> 18) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C2;
	n = (s >> 48) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C3;
	n = (s >>  0) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C4;
	n = (s >> 12) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C5;
	n = (s >> 36) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C6;
	n = (s >> 54) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C7;
	n = (s >> 28) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C8;
	n = (s >> 10) & 63; t = ((t << n) | (t >> (64 - n))); t ^= C9;
	return t;
}

UInt32 Media::SSRC::SleefRNG::Add64c(UInt64 *ret, UInt64 *u, UInt64 *v)
{
	UInt64 w;
	UInt32 c;

	w = *u + *v;
	c = w < *u;
	*ret = w;

	return c;
}

void Media::SSRC::SleefRNG::Add128(U128 *r, U128 *u, U128 *v)
{
	UInt64 rl;
	UInt64 ru;

	ru = u->u + v->u + Add64c(&rl, &u->l, &v->l);

	r->l = rl;
	r->u = ru;
}

Media::SSRC::SleefRNG::U128 Media::SSRC::SleefRNG::NextState(U128 state)
{
	U128 ret, u;
	u.u = 0xE7866D8CFFF116AAULL;
	u.l = 0xA933E07E1CB7963DULL;
	Add128(&ret, &state, &u);
	return ret;
}

Media::SSRC::SleefRNG::SleefRNG(UInt64 seed)
{
	this->state.l = 0xAD62418D14EA8247ULL * seed + 0x01C4B4886CC66F59ULL;
	this->state.u = 0xFFD1390A0ADC2FB8ULL * seed + 0xDABBB8174D95C99BULL;
	this->remain = 0;
	this->remainSize = 0;
}

Media::SSRC::SleefRNG::~SleefRNG()
{
}

UInt64 Media::SSRC::SleefRNG::Next(UInt32 nbits)
{
	if (nbits > this->remainSize)
	{
		this->remain = this->Xrandom64();
		this->remainSize = 64;
	}

	UInt64 ret = this->remain & ~(-1LL << nbits);
	this->remain >>= nbits;
	this->remainSize -= nbits;
	return ret;
}

UInt64 Media::SSRC::SleefRNG::Next64()
{
	return this->Xrandom64();
}

void Media::SSRC::SleefRNG::NextBytes(UInt8 *ptr, UOSInt z)
{
	UOSInt i = 0;
	while (i < (z & ~7))
	{
		WriteNUInt64(&ptr[i], this->Xrandom64());
		i += 8;
	}

	while (i < z)
	{
		ptr[i] = (UInt8)this->Next(8);
		i++;
	}
}

Double Media::SSRC::SleefRNG::NextDouble()
{
	return (Double)this->Xrandom64() * (1.0 / (1ULL << 32) / (1ULL << 32));
}

Double Media::SSRC::SleefRNG::NextRectangularDouble(Double min, Double max)
{
	return min + (Double)this->Xrandom64() * (1.0 / (1ULL << 32) / (1ULL << 32)) * (max - min);
}

void Media::SSRC::SleefRNG::FillRectangularDouble(Double *ptr, UOSInt z, Double min, Double max)
{
	UOSInt i = 0;
	while (i < z)
	{
		ptr[i] = min + (Double)this->Xrandom64() * (1.0 / (1ULL << 32) / (1ULL << 32)) * (max - min);
		i++;
	}
}

Double Media::SSRC::SleefRNG::NextTriangularDouble(Double peak)
{
	return ((Double)this->Xrandom64() - (Double)this->Xrandom64()) * (1.0 / (1ULL << 32) / (1ULL << 32)) * peak;
}

void Media::SSRC::SleefRNG::FillTriangularDouble(Double *ptr, UOSInt z, Double peak)
{
	UOSInt i = 0;
	while (i < z)
	{
		ptr[i] = ((Double)this->Xrandom64() - (Double)this->Xrandom64()) * (1.0 / (1ULL << 32) / (1ULL << 32)) * peak;
		i++;
	}
}

Double Media::SSRC::SleefRNG::NextTwoLevelDouble(Double peak)
{
	return (this->Next(1) != 0) ? -peak : peak;
}

void Media::SSRC::SleefRNG::FillTwoLevelDouble(Double *ptr, UOSInt z, Double peak)
{
	UOSInt i = 0;
	while (i < z)
	{
		ptr[i] = (this->Next(1) != 0) ? -peak : peak;
		i++;
	}
}
