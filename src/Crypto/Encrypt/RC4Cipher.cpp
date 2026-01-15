#include "Stdafx.h"
#include "Crypto/Encrypt/RC4Cipher.h"

Crypto::Encrypt::RC4Cipher::RC4Cipher(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	this->SetKey(key, keyLen);
}

Crypto::Encrypt::RC4Cipher::~RC4Cipher()
{
}

void Crypto::Encrypt::RC4Cipher::SetKey(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	UIntOS i = 0;
	while (i < 256)
	{
		this->s[i] = (UInt8)i;
		i++;
	}
	UInt8 tmp;
	UInt8 j = 0;
	if (keyLen == 4)
	{
		i = 0;
		while (i < 256)
		{
			j = (UInt8)(j + this->s[i] + key[i & 3]);
			tmp = this->s[i];
			this->s[i] = this->s[j];
			this->s[j] = tmp;
			i++;
		}
	}
	else
	{
		i = 0;
		while (i < 256)
		{
			j = (UInt8)(j + this->s[i] + key[i % keyLen]);
			tmp = this->s[i];
			this->s[i] = this->s[j];
			this->s[j] = tmp;
			i++;
		}
	}
	this->i = 0;
	this->j = 0;
}

UIntOS Crypto::Encrypt::RC4Cipher::Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UIntOS lengLeft = inSize;
	UInt8 tmp1;
	UInt8 tmp2;
	while (lengLeft-- > 0)
	{
		this->i++;
		this->j = (UInt8)(this->j + this->s[i]);
		tmp1 = this->s[i];
		tmp2 = this->s[j];
		this->s[i] = tmp2;
		this->s[j] = tmp1;
		*outBuff++ = (UInt8)(*inBuff ^ s[(tmp1 + tmp2) & 255]);
		inBuff++;
	}
	return inSize;
}

UIntOS Crypto::Encrypt::RC4Cipher::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UIntOS lengLeft = inSize;
	UInt8 tmp1;
	UInt8 tmp2;
	while (lengLeft-- > 0)
	{
		this->i++;
		this->j = (UInt8)(this->j + this->s[i]);
		tmp1 = this->s[i];
		tmp2 = this->s[j];
		this->s[i] = tmp2;
		this->s[j] = tmp1;
		*outBuff++ = (UInt8)(*inBuff ^ s[(tmp1 + tmp2) & 255]);
		inBuff++;
	}
	return inSize;
}

UIntOS Crypto::Encrypt::RC4Cipher::GetEncBlockSize() const
{
	return 1;
}

UIntOS Crypto::Encrypt::RC4Cipher::GetDecBlockSize() const
{
	return 1;
}
