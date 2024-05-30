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
	this->aes.EncryptBlock(zero, l);
	i = 0;
	while (i < 15)
	{
		this->k1[i] = (UInt8)((l[i] << 1) | (l[i + 1] >> 7));
		i++;
	}
	if (l[0] & 0x80)
	{
		this->k1[15] = (UInt8)((l[15] << 1) ^ 0x87);
	}
	else
	{
		this->k1[15] = (UInt8)(l[15] << 1);
	}

	i = 0;
	while (i < 15)
	{
		this->k2[i] = (UInt8)((this->k1[i] << 1) | (this->k1[i + 1] >> 7));
		i++;
	}
	if (this->k1[0] & 0x80)
	{
		this->k2[15] = (UInt8)((this->k1[15] << 1) ^ 0x87);
	}
	else
	{
		this->k2[15] = (UInt8)(this->k1[15] << 1);
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

UnsafeArray<UTF8Char> Crypto::Hash::AESCMAC::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("AES-CMAC"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::AESCMAC::Clone() const
{
	NN<Crypto::Hash::IHash> ret;
	NEW_CLASSNN(ret, Crypto::Hash::AESCMAC(this->key));
	return ret;
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
			this->aes.EncryptBlock(this->x, this->x);
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
		this->aes.EncryptBlock(this->x, this->x);
		buff += 16;
		buffSize -= 16;
	}
	MemCopyNO(this->buff, buff, buffSize);
	this->buffSize = buffSize;
}

void Crypto::Hash::AESCMAC::GetValue(UnsafeArray<UInt8> buff) const
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
		this->aes.EncryptBlock(y, buff.Ptr());
	}
	else if (this->buffSize == 16)
	{
		WriteNUInt64(&y[0], ReadNUInt64(&this->x[0]) ^ ReadNUInt64(&this->buff[0]) ^ ReadNUInt64(&this->k1[0]));
		WriteNUInt64(&y[8], ReadNUInt64(&this->x[8]) ^ ReadNUInt64(&this->buff[8]) ^ ReadNUInt64(&this->k1[8]));
		this->aes.EncryptBlock(y, buff.Ptr());
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
		this->aes.EncryptBlock(y, buff.Ptr());
	}
}

UOSInt Crypto::Hash::AESCMAC::GetBlockSize() const
{
	return 16;
}

UOSInt Crypto::Hash::AESCMAC::GetResultSize() const
{
	return 16;
}
