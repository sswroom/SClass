#include "Stdafx.h"
#include "Crypto/Encrypt/XOREnc.h"

Crypto::Encrypt::XOREnc::XOREnc()
{
}

Crypto::Encrypt::XOREnc::~XOREnc()
{
}

OSInt Crypto::Encrypt::XOREnc::Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam)
{
	UInt8 lastByte = 0;
	OSInt i = 0;
	while (i < inSize)
	{
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = outBuff[i++];
	}
	return inSize;
}

OSInt Crypto::Encrypt::XOREnc::Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam)
{
	UInt8 lastByte = 0;
	UInt8 lastLastByte = 0;
	OSInt i = 0;
	while (i < inSize)
	{
		lastLastByte = inBuff[i];
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = lastLastByte;
		i++;
	}
	return inSize;
}

OSInt Crypto::Encrypt::XOREnc::GetEncBlockSize()
{
	return 1;
}

OSInt Crypto::Encrypt::XOREnc::GetDecBlockSize()
{
	return 1;
}
