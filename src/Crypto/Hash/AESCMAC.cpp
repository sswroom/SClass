#include "Stdafx.h"
#include "Crypto/Hash/AESCMAC.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

//RFC4493
void Crypto::Hash::AESCMAC::GenSubKey()
{
	UInt8 zero[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	UInt8 l[16];
	UOSInt i;
	this->aes.EncryptBlock(zero, l, 0);
	i = 0;
	while (i < 15)
	{
		this->k1[i] = (l[i] << 1) | (l[i + 1] >> 7);
		i++;
	}
	if (l[0] & 0x80)
	{
		this->k1[15] = (l[15] << 1) ^ 0x87;
	}
	else
	{
		this->k1[15] = l[15] << 1;
	}

	i = 0;
	while (i < 15)
	{
		this->k2[i] = (this->k1[i] << 1) | (this->k1[i + 1] >> 7);
		i++;
	}
	if (this->k1[0] & 0x80)
	{
		this->k2[15] = (this->k1[15] << 1) ^ 0x87;
	}
	else
	{
		this->k2[15] = this->k1[15] << 1;
	}
}

Crypto::Hash::AESCMAC::AESCMAC(const UInt8 *key) : aes(key)
{
	MemCopyNO(this->key, key, 16);
	this->GenSubKey();
	this->Clear();
}

Crypto::Hash::AESCMAC::~AESCMAC()
{

}

UTF8Char *Crypto::Hash::AESCMAC::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("AES-CMAC"));
}

Crypto::Hash::IHash *Crypto::Hash::AESCMAC::Clone()
{
	return NEW_CLASS_D(Crypto::Hash::AESCMAC(this->key));
}

void Crypto::Hash::AESCMAC::Clear()
{
	MemClear(this->x, 16);
	this->buffSize = 0;
}

void Crypto::Hash::AESCMAC::Calc(const UInt8 *buff, UOSInt buffSize)
{
	if (this->buffSize != 0)
	{
		if (this->buffSize + buffSize > 16)
		{
			MemCopyNO(&this->buff[this->buffSize], buff, 16 - this->buffSize);
			WriteNUInt64(&this->x[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&this->buff[0]));
			WriteNUInt64(&this->x[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&this->buff[8]));
			this->aes.EncryptBlock(this->x, this->x, 0);
			buff += 16 - this->buffSize;
			buffSize -= (16 - this->buffSize);
			this->buffSize = 0;
		}
		else
		{
			MemCopyNO(&this->buff[this->buffSize], buff, buffSize);
			this->buffSize += buffSize;
			return;
		}
	}

	while (buffSize > 16)
	{
		WriteNUInt64(&this->x[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&buff[0]));
		WriteNUInt64(&this->x[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&buff[8]));
		this->aes.EncryptBlock(this->x, this->x, 0);
		buff += 16;
		buffSize -= 16;
	}
	MemCopyNO(this->buff, buff, buffSize);
	this->buffSize = buffSize;
}

void Crypto::Hash::AESCMAC::GetValue(UInt8 *buff)
{
	UInt8 y[16];
	UOSInt i;
	if (this->buffSize == 0)
	{
		WriteNUInt64(&y[0], 0);
		WriteNUInt64(&y[8], 0);
		y[0] = 0x80;
		WriteNUInt64(&y[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&y[0]) ^ ReadNUInt64(&this->k2[0]));
		WriteNUInt64(&y[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&y[8]) ^ ReadNUInt64(&this->k2[8]));
		this->aes.EncryptBlock(y, buff, 0);
	}
	else if (this->buffSize == 16)
	{
		WriteNUInt64(&y[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&this->buff[0]) ^ ReadNUInt64(&this->k1[0]));
		WriteNUInt64(&y[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&this->buff[8]) ^ ReadNUInt64(&this->k1[8]));
		this->aes.EncryptBlock(y, buff, 0);
	}
	else
	{
		MemCopyNO(y, this->buff, this->buffSize);
		i = this->buffSize;
		y[i] = 0x80;
		i++;
		while (i < 16)
		{
			y[i++] = 0;
		}
		WriteNUInt64(&y[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&y[0]) ^ ReadNUInt64(&this->k2[0]));
		WriteNUInt64(&y[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&y[8]) ^ ReadNUInt64(&this->k2[8]));
		this->aes.EncryptBlock(y, buff, 0);
	}
}

UOSInt Crypto::Hash::AESCMAC::GetBlockSize()
{
	return 16;
}

UOSInt Crypto::Hash::AESCMAC::GetResultSize()
{
	return 16;
}