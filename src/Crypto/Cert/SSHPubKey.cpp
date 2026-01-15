#include "Stdafx.h"
#include "Crypto/Cert/SSHPubKey.h"

Crypto::Cert::SSHPubKey::SSHPubKey(Text::CStringNN sourceName, Data::ByteArrayR buff) : buff(buff)
{
	this->sourceNameObj = Text::String::New(sourceName);
}

Crypto::Cert::SSHPubKey::~SSHPubKey()
{
	this->sourceNameObj->Release();
}

UnsafeArray<const UInt8> Crypto::Cert::SSHPubKey::GetArr() const
{
	return this->buff.Arr();
}

UOSInt Crypto::Cert::SSHPubKey::GetSize() const
{
	return this->buff.GetSize();
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::SSHPubKey::GetRSAModulus(OptOut<UOSInt> size) const
{
	if (this->buff.GetSize() < 15)
		return nullptr;
	UOSInt i;
	UInt32 buffSize;
	if (this->buff.ReadMU32(0) != 7 || !Text::CStringNN(&this->buff[4], 7).Equals(CSTR("ssh-rsa")))
		return nullptr;
	buffSize = this->buff.ReadMU32(11);
	i = 15;
	if (this->buff.GetSize() < i + buffSize + 4)
		return nullptr;
	i += buffSize + 4;
	buffSize = this->buff.ReadMU32(i - 4);
	if (this->buff.GetSize() < i + buffSize)
		return nullptr;
	size.Set(buffSize);
	return &this->buff[i];	
}

UnsafeArrayOpt<const UInt8> Crypto::Cert::SSHPubKey::GetRSAPublicExponent(OptOut<UOSInt> size) const
{
	if (this->buff.GetSize() < 15)
		return nullptr;
	UOSInt i;
	UInt32 buffSize;
	if (this->buff.ReadMU32(0) != 7 || !Text::CStringNN(&this->buff[4], 7).Equals(CSTR("ssh-rsa")))
		return nullptr;
	buffSize = this->buff.ReadMU32(11);
	i = 15;
	if (this->buff.GetSize() < i + buffSize + 4)
		return nullptr;
	size.Set(buffSize);
	return &this->buff[i];
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::SSHPubKey::CreateKey() const
{
	UnsafeArray<const UInt8> modulus;
	UOSInt modulusSize;
	UnsafeArray<const UInt8> publicExponent;
	UOSInt publicExponentSize;
	if (GetRSAModulus(modulusSize).SetTo(modulus) && GetRSAPublicExponent(publicExponentSize).SetTo(publicExponent))
	{
		return Crypto::Cert::X509Key::CreateRSAPublicKey(this->sourceNameObj->ToCString(), Data::ByteArrayR(modulus, modulusSize), Data::ByteArrayR(publicExponent, publicExponentSize));
	}
	return nullptr;
}

NN<Crypto::Cert::SSHPubKey> Crypto::Cert::SSHPubKey::CreateRSAPublicKey(Text::CStringNN name, Data::ByteArrayR modulus, Data::ByteArrayR publicExponent)
{
	static UInt8 header[] = {0x00, 0x00, 0x00, 0x07, 0x73, 0x73, 0x68, 0x2D, 0x72, 0x73, 0x61};
	UnsafeArray<UInt8> buff = MemAllocArr(UInt8, modulus.GetSize() + publicExponent.GetSize() + 19);
	UOSInt i;
	MemCopyNO(buff.Ptr(), header, sizeof(header));
	i = sizeof(header);
	WriteMUInt32(&buff[i], publicExponent.GetSize());
	MemCopyNO(&buff[i + 4], publicExponent.Ptr(), publicExponent.GetSize());
	i += 4 + publicExponent.GetSize();
	WriteMUInt32(&buff[i], modulus.GetSize());
	MemCopyNO(&buff[i + 4], modulus.Ptr(), modulus.GetSize());
	i += 4 + modulus.GetSize();
	NN<SSHPubKey> key;
	NEW_CLASSNN(key, SSHPubKey(name, Data::ByteArrayR(buff, i)));
	MemFreeArr(buff);
	return key;
}

Bool Crypto::Cert::SSHPubKey::IsValid(Data::ByteArrayR buff)
{
	static UInt8 header1[] = {0x00, 0x00, 0x00, 0x07, 0x73, 0x73, 0x68, 0x2D, 0x72, 0x73, 0x61, 0x00, 0x00, 0x00};
	if (buff.StartsWith(Data::ByteArrayR(header1, sizeof(header1)))) return true;
	return false;
}
