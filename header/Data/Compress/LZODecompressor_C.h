#ifndef _SM_DATA_COMPRESS_LZODECOMPRESSOR_C
#define _SM_DATA_COMPRESS_LZODECOMPRESSOR_C
extern "C"
{
	Bool LZODecompressor_Decompress(const UInt8 *in, UOSInt in_len, UInt8 *out, UOSInt *out_len);
	UOSInt LZODecompressor_CalcDecSize(const UInt8 *in, UOSInt in_len);
}
#endif
