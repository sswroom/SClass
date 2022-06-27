#include "Stdafx.h"
#include "Crypto/Encrypt/XOREnc.h"

Crypto::Encrypt::XOREnc::XOREnc()
{
}

Crypto::Encrypt::XOREnc::~XOREnc()
{
}

UOSInt Crypto::Encrypt::XOREnc::Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam)
{
	UInt8 lastByte = 0;
	UOSInt i = 0;
	while (i < inSize)
	{
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = outBuff[i++];
	}
	return inSize;
}

UOSInt Crypto::Encrypt::XOREnc::Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam)
{
	UInt8 lastByte = 0;
	UInt8 lastLastByte = 0;
	UOSInt i = 0;
	while (i < inSize)
	{
		lastLastByte = inBuff[i];
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = lastLastByte;
		i++;
	}
	return inSize;
}

UOSInt Crypto::Encrypt::XOREnc::GetEncBlockSize() const
{
	return 1;
}

UOSInt Crypto::Encrypt::XOREnc::GetDecBlockSize() const
{
	return 1;
}
