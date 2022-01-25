#ifndef _SM_CRYPTO_TOTP
#define _SM_CRYPTO_TOTP
#include "Crypto/OTP.h"

namespace Crypto
{
	class TOTP : public OTP
	{
	private:
		UInt8 *key;
		UOSInt keySize;
		UOSInt intervalMS;

	public:
		TOTP(const UInt8 *key, UOSInt keySize);
		virtual ~TOTP();

		virtual OTPType GetType();
		virtual UInt64 GetCounter();
		virtual UInt32 NextCode();
		virtual Bool IsValid(UInt32 code);
		virtual void GenURI(Text::StringBuilderUTF8 *sb, const UTF8Char *name);
	};
}
#endif
