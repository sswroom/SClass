#ifndef _SM_MEDIA_IMAGEUTIL_C
#define _SM_MEDIA_IMAGEUTIL_C

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
	void ImageUtil_ImageColorBuffer32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UOSInt buffSize);
	void ImageUtil_ImageColorFill32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt32 col);
	void ImageUtil_ImageColorBlend32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt32 col);
	void ImageUtil_ImageFillAlpha32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt8 a);
	void ImageUtil_ImageAlphaMul32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt32 a); //65536 = 1.0
	void ImageUtil_ImageColorMul32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt32 c);
	void ImageUtil_DrawRectNA32(UInt8 *pixelPtr, UOSInt w, UOSInt h, UOSInt bpl, UInt32 col); //no alpha
	void ImageUtil_ConvP1_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP1_A1_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_A1_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_A1_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_A1_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvB5G5R5_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB5G6R5_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8A8_B8G8R8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8A8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8N8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB16G16R16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR16G16B16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB16G16R16A16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR16G16B16A16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvA2B10G10R10_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32A32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFR32G32B32A32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFR32G32B32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32A32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvP1_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP1_A1_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP2_A1_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP4_A1_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvP8_A1_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt storeW, OSInt dbpl, const UInt8 *pal);
	void ImageUtil_ConvB5G5R5_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB5G6R5_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR8G8B8A8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB8G8R8A8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvB16G16R16_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR16G16B16_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvR16G16B16A16_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvA2B10G10R10_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32A32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFR32G32B32A32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFB32G32R32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFR32G32B32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32A32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvFW32_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16A16_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW8A8_B8G8R8A8(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW16A16_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_ConvW8A8_B16G16R16A16(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt w, UOSInt h, OSInt sbpl, OSInt dbpl);
	void ImageUtil_Rotate32_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_Rotate32_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_Rotate32_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_Rotate64_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_Rotate64_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_Rotate64_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFlip32(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
	void ImageUtil_HFRotate32_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFRotate32_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFRotate32_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFlip64(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
	void ImageUtil_HFRotate64_CW90(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFRotate64_CW180(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);
	void ImageUtil_HFRotate64_CW270(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt srcWidth, UOSInt srcHeight, UOSInt sbpl, UOSInt dbpl);

	void ImageUtil_CopyShiftW(const UInt8 *srcPtr, UInt8 *destPtr, UOSInt byteSize, UOSInt shiftCnt); //Assume aligned
	void ImageUtil_UVInterleaveShiftW(UInt8 *destPtr, const UInt8 *uptr, const UInt8 *vptr, UOSInt pixelCnt, UOSInt shiftCnt); //Assume aligned
	void ImageUtil_YUV_Y416ShiftW(UInt8 *destPtr, const UInt8 *yptr, const UInt8 *uptr, const UInt8 *vptr, UOSInt pixelCnt, UOSInt shiftCnt); //Assume aligned
}
#endif
