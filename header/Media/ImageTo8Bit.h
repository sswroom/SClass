#ifndef _SM_MEDA_IMAGETO8BIT
#define _SM_MEDA_IMAGETO8BIT
namespace Media
{
	class ImageTo8Bit
	{
	public:
		static void From32bpp(UnsafeArray<UInt8> src, UnsafeArray<UInt8> dest, UnsafeArray<UInt8> palette, UOSInt width, UOSInt height, OSInt sbpl, OSInt dbpl);
	};
}
#endif
