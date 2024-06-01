#include "Stdafx.h"
#include "Crypto/Encrypt/XOREnc.h"

Crypto::Encrypt::XOREnc::XOREnc()
{
}

Crypto::Encrypt::XOREnc::~XOREnc()
{
}

UOSInt Crypto::Encrypt::XOREnc::Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
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

UOSInt Crypto::Encrypt::XOREnc::Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
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
