#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"

#define SHA1HashSize 20
#define SHA1CircularShift(y, x) (((x) << (y)) | ((x) >> (32-(y))))

extern "C" void SHA1_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block)
{
	UInt32 W[80];
	const UInt32 K[] =    {
								0x5A827999,
								0x6ED9EBA1,
								0x8F1BBCDC,
								0xCA62C1D6};
	OSInt           t;
	UInt32      temp;
	UInt32      A, B, C, D, E;

	for(t = 0; t < 16; t++)
	{
		W[t] = ReadMUInt32(&Message_Block[t * 4]);
	}

	for(t = 16; t < 80; t++)
	{
		W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
	}

	A = Intermediate_Hash[0];
	B = Intermediate_Hash[1];
	C = Intermediate_Hash[2];
	D = Intermediate_Hash[3];
	E = Intermediate_Hash[4];

	for(t = 0; t < 20; t++)
	{
		temp =  SHA1CircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t] + K[0];
		E = D;
		D = C;
		C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

	for(t = 20; t < 40; t++)
	{
		temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
		E = D;
		D = C;
		C = SHA1CircularShift(30,B);
		B = A;
		A = temp;
	}

	for(t = 40; t < 60; t++)
	{
		temp = SHA1CircularShift(5,A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }
	
	Intermediate_Hash[0] += A;
	Intermediate_Hash[1] += B;
	Intermediate_Hash[2] += C;
	Intermediate_Hash[3] += D;
	Intermediate_Hash[4] += E;
}
