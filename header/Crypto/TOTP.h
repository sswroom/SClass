#ifndef _SM_CRYPTO_TOTP
#define _SM_CRYPTO_TOTP

namespace Crypto
{
	class TOTP
	{
	public:
		TOTP(const UInt8 *key);
		~TOTP();
	};
}
#endif
