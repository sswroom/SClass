#ifndef _SM_CRYPTO_OTP
#define _SM_CRYPTO_OTP
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	class OTP
	{
	public:
		enum class OTPType
		{
			HOTP,
			TOTP
		};

	protected:
		UInt32 nDigits;

	public:
		OTP(UInt32 nDigits);
		virtual ~OTP();

		virtual OTPType GetType() = 0;
		virtual UInt64 GetCounter() = 0;
		virtual UInt32 NextCode() = 0;
		virtual Bool IsValid(UInt32 code) = 0;
		virtual void GenURI(Text::StringBuilderUTF8 *sb, const UTF8Char *name) = 0;

		UTF8Char *CodeString(UTF8Char *sbuff, UInt32 code);
		static const UTF8Char *OTPTypeGetName(OTPType type);
	};
}
#endif
