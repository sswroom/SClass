#ifndef _SM_CRYPTO_HASH_CRC32R_C
#define _SM_CRYPTO_HASH_CRC32R_C
extern "C"
{
	UInt32 CRC32R_Reverse(UInt32 polynomial);
	UInt32 CRC32R_Calc(const UInt8 *buff, UIntOS buffSize, UInt32 *tab, UInt32 currVal);
}
#endif
