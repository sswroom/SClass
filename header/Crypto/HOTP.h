#ifndef _SM_CRYPTO_HOTP
#define _SM_CRYPTO_HOTP

namespace Crypto
{
	class HOTP
	{
	private:
		UInt8 *key;
		UOSInt keySize;
		UInt64 counter;
		UInt32 nDigits;

	public:
		HOTP(const UInt8 *key, UOSInt keySize, UInt64 counter);
		~HOTP();

		UInt64 GetCounter();
		UInt32 NextCode();
		Bool IsValid(UInt64 code);

		static UInt32 CalcCode(const UInt8 *key, UOSInt keySize, UInt64 counter, UInt32 nDigits);
	};
}
#endif
