#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNCC
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNCC

extern "C"
{
	void LanczosResizerFunc_HorizontalFilterB8G8R8A8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterB8G8R8A8PA(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterPal8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_VerticalFilterB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ExpandB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ExpandB8G8R8A8PA(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ExpandPal8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_CollapseB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ImgCopyB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ImgCopyB8G8R8A8PA(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
	void LanczosResizerFunc_ImgCopyPal8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbTable);
}

#endif
