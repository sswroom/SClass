#ifndef _SM_MEDIA_IMAGECOPY_C
#define _SM_MEDIA_IMAGECOPY_C
extern "C"
{
	void ImageCopy_ImgCopyR(const UInt8 *inPt, UInt8 *outPt, UOSInt copySize, UOSInt height, UOSInt sbpl, UOSInt dbpl, Bool upsideDown);
	void ImageCopy_ImgCopy(const UInt8 *inPt, UInt8 *outPt, UOSInt copySize, UOSInt height, OSInt sstep, OSInt dstep);
}
#endif
