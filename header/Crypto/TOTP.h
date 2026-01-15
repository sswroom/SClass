#ifndef _SM_CRYPTO_TOTP
#define _SM_CRYPTO_TOTP
#include "Crypto/OTP.h"

namespace Crypto
{
	class TOTP : public OTP
	{
	private:
		UnsafeArray<UInt8> key;
		UIntOS keySize;
		UIntOS intervalMS;

	public:
		TOTP(UnsafeArray<const UInt8> key, UIntOS keySize);
		virtual ~TOTP();

		virtual OTPType GetType();
		virtual UInt64 GetCounter();
		virtual UInt32 NextCode();
		virtual Bool IsValid(UInt32 code);
		virtual void GenURI(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> name);
	};
}
#endif
