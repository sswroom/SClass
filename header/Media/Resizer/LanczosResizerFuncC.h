#ifndef _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNCC
#define _SM_MEDIA_RESIZER_LANCZOSRESIZERFUNCC

extern "C"
{
	void LanczosResizerFunc_HorizontalFilterB8G8R8A8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterR8G8B8A8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterB8G8R8A8PA(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterR8G8B8A8PA(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterB8G8R8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterR8G8B8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgbaTable, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterPal8(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *pal8Table, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_HorizontalFilterR16G16B16(const UInt8 *inPt, UInt8 *outPt,UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *rgba16Table, UOSInt swidth, UInt8 *tmpbuff);
	void LanczosResizerFunc_VerticalFilterB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable);
	void LanczosResizerFunc_VerticalFilterB8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, UOSInt tap, OSInt *index, Int64 *weight, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable);
	void LanczosResizerFunc_ExpandB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandR8G8B8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandB8G8R8A8PA(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandR8G8B8A8PA(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandB8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandR8G8B8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgbaTable);
	void LanczosResizerFunc_ExpandPal8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *pal8Table);
	void LanczosResizerFunc_ExpandR16G16B16(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *rgba16Table);
	void LanczosResizerFunc_CollapseB8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable);
	void LanczosResizerFunc_CollapseB8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable);
	void LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8A8_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8A8PA_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyPal8_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *pal8Table);
	void LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8A8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgba16Table);
	void LanczosResizerFunc_ImgCopyB8G8R8A8_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8A8_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyB8G8R8A8PA_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8A8PA_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyB8G8R8_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyR8G8B8_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgbaTable);
	void LanczosResizerFunc_ImgCopyPal8_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *pal8Table);
	void LanczosResizerFunc_ImgCopyR16G16B16_B8G8R8(const UInt8 *inPt, UInt8 *outPt, UOSInt width, UOSInt height, OSInt sstep, OSInt dstep, UInt8 *lrbgraTable, UInt8 *rgba16Table);
}

#endif
