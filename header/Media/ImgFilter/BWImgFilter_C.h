#ifndef _SM_MEDIA_IMAGEFILTER_BWIMGFILTER_C
#define _SM_MEDIA_IMAGEFILTER_BWIMGFILTER_C
extern "C"
{
	void BWImgFilter_ProcYUY2(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcUYVY(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcAYUV(UInt8 *imgData, UOSInt pxCnt);
	void BWImgFilter_ProcY416(UInt8 *imgData, UOSInt pxCnt);
}
#endif
