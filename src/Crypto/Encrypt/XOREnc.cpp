#include "Stdafx.h"
#include "Crypto/Encrypt/XOREnc.h"

Crypto::Encrypt::XOREnc::XOREnc()
{
}

Crypto::Encrypt::XOREnc::~XOREnc()
{
}

UIntOS Crypto::Encrypt::XOREnc::Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 lastByte = 0;
	UIntOS i = 0;
	while (i < inSize)
	{
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = outBuff[i++];
	}
	return inSize;
}

UIntOS Crypto::Encrypt::XOREnc::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 lastByte = 0;
	UInt8 lastLastByte = 0;
	UIntOS i = 0;
	while (i < inSize)
	{
		lastLastByte = inBuff[i];
		outBuff[i] = inBuff[i] ^ lastByte;
		lastByte = lastLastByte;
		i++;
	}
	return inSize;
}

UIntOS Crypto::Encrypt::XOREnc::GetEncBlockSize() const
{
	return 1;
}

UIntOS Crypto::Encrypt::XOREnc::GetDecBlockSize() const
{
	return 1;
}
