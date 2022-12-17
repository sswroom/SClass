#include "Stdafx.h"
#include "Crypto/Encrypt/RSACipher.h"

UOSInt Crypto::Encrypt::RSACipher::PaddingRemove(UInt8* destBuff, const UInt8* blockWithPadding, Padding padding)
{
	MemCopyNO(destBuff, blockWithPadding, 256);
	return 256;
}

Text::CString Crypto::Encrypt::RSACipher::PaddingGetName(Padding rsaPadding)
{
	switch (rsaPadding)
	{
	case Padding::PKCS1:
		return CSTR("PKCS1");
	case Padding::NoPadding:
		return CSTR("NoPadding");
	case Padding::PKCS1_OAEP:
		return CSTR("PKCS1_OAEP");
	case Padding::X931:
		return CSTR("X931");
	case Padding::PKCS1_PSS:
		return CSTR("PKCS1_PSS");
	case Padding::PKCS1_WithTLS:
		return CSTR("PKCS1_WithTLS");
	default:
		return CSTR("Unknown");
	}
}
