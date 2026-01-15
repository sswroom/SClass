#ifndef _SM_MEDIA_IMAGEFILTER_BGIMGFILTER_C
#define _SM_MEDIA_IMAGEFILTER_BGIMGFILTER_C
extern "C"
{
	void BGImgFilter_DiffA8_YUY2(UInt8 *destImg, UInt8 *bgImg, UIntOS pxCnt);
	void BGImgFilter_DiffYUV8(UInt8 *destImg, UInt8 *bgImg, UIntOS byteCnt);
}
#endif
