#ifndef _SM_CRYPTO_HASH_CRC16R_C
#define _SM_CRYPTO_HASH_CRC16R_C
extern "C"
{
	void CRC16R_InitTable(UInt16 *tab, UInt16 rpn);
	UInt16 CRC16R_Calc(const UInt8 *buff, UOSInt buffSize, UInt16 *tab, UInt16 currVal);
}
#endif
