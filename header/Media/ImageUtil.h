#ifndef _SM_MEDIA_IMAGEUTIL
#define _SM_MEDIA_IMAGEUTIL

extern "C"
{
	void ImageUtil_SwapRGB(UInt8 *pixelPtr, UOSInt pixelCnt, UOSInt bpp);
	void ImageUtil_ColorReplace32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UInt32 col); //Replace non-zero color to specific color
	void ImageUtil_ColorReplace32A(UInt8 *pixelPtr, UOSInt w, UOSInt h, UInt32 col); //Replace non-zero color to specific color with depth
	void ImageUtil_ColorReplace32A2(UInt8 *pixelPtr, UOSInt w, UOSInt h, UInt32 col); //Replace non-zero color to specific color with depth type 2
	void ImageUtil_ColorFill32(UInt8 *pixelPtr, UOSInt pixelCnt, UInt32 color);
	void ImageUtil_ImageColorReplace32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, UInt32 col); //Replace non-zero color to specific color
	void ImageUtil_ImageMaskABlend32(const UInt8 *maskPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, UInt32 col);
	void ImageUtil_ImageMask2ABlend32(const UInt8 *maskPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, UInt32 col1, UInt32 col2);
	void ImageUtil_ImageColorBuffer32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, OSInt buffSize);
	void ImageUtil_ImageColorFill32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt32 col);
	void ImageUtil_ImageColorBlend32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt32 col);
	void ImageUtil_ImageFillAlpha32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt8 a);
	void ImageUtil_ImageAlphaMul32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt32 a); //65536 = 1.0
	void ImageUtil_ImageColorMul32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt32 c);
	void ImageUtil_DrawRectNA32(UInt8 *pixelPtr, UOSInt w, UOSInt h, OSInt bpl, UInt32 col); //no alpha
	void ImageUtil_ConvP1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP1_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_A1_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvB5G5R5_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB5G6R5_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8A8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8N8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvARGB48_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvARGB64_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvA2B10G10R10_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32A32_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32A32_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32_32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvP1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP1_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_A1_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvB5G5R5_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB5G6R5_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvARGB32_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvARGB48_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvA2B10G10R10_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32A32_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32A32_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32_64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16A16_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW8A8_ARGB32(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16A16_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW8A8_ARGB64(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate32_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate32_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate32_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate64_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate64_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate64_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, OSInt sbpl, OSInt dbpl);

	void ImageUtil_CopyShiftW(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt byteSize, UOSInt shiftCnt); //Assume aligned
	void ImageUtil_UVInterleaveShiftW(UInt8 *destPtr, const UInt8 *uptr, const UInt8 *vptr, UOSInt pixelCnt, UOSInt shiftCnt); //Assume aligned
	void ImageUtil_YUV_Y416ShiftW(UInt8 *destPtr, const UInt8 *yptr, const UInt8 *uptr, const UInt8 *vptr, UOSInt pixelCnt, UOSInt shiftCnt); //Assume aligned
}

#endif
