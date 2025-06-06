#ifndef _SM_CRYPTO_HASH_ADLER32_C
#define _SM_CRYPTO_HASH_ADLER32_C
extern "C"
{
	UInt32 Adler32_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 abVal);
}
#endif
