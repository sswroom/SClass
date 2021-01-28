#ifndef _SM_MEDA_IMAGETO8BIT
#define _SM_MEDA_IMAGETO8BIT
namespace Media
{
	class ImageTo8Bit
	{
	public:
		static void From32bpp(UInt8 *src, UInt8 *dest, UInt8 *palette, OSInt width, OSInt height, OSInt sbpl, OSInt dbpl);
	};
};
#endif
