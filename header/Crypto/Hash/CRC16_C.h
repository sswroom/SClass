#ifndef _SM_CRYPTO_HASH_CRC16_C
#define _SM_CRYPTO_HASH_CRC16_C
extern "C"
{
	void CRC16_InitTable(UInt16 *tab, UInt16 polynomial);
	UInt16 CRC16_Calc(const UInt8 *buff, UIntOS buffSize, UInt16 *tab, UInt16 currVal);
}
#endif
