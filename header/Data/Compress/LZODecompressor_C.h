#ifndef _SM_DATA_COMPRESS_LZODECOMPRESSOR_C
#define _SM_DATA_COMPRESS_LZODECOMPRESSOR_C
extern "C"
{
	Bool LZODecompressor_Decompress(const UInt8 *in, UIntOS in_len, UInt8 *out, UIntOS *out_len);
	UIntOS LZODecompressor_CalcDecSize(const UInt8 *in, UIntOS in_len);
}
#endif
