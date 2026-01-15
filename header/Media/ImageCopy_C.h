#ifndef _SM_MEDIA_IMAGECOPY_C
#define _SM_MEDIA_IMAGECOPY_C
extern "C"
{
	void ImageCopy_ImgCopyR(const UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, UIntOS sbpl, UIntOS dbpl, Bool upsideDown);
	void ImageCopy_ImgCopy(const UInt8 *inPt, UInt8 *outPt, UIntOS copySize, UIntOS height, IntOS sstep, IntOS dstep);
}
#endif
