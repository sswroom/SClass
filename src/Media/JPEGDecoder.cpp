#include "Stdafx.h"
#include "Media/JPEGDecoder.h"
#include <stddef.h>
#include <jpeglib.h>
#include <setjmp.h>

Media::JPEGDecoder::JPEGDecoder()
{
}

Media::JPEGDecoder::~JPEGDecoder()
{
}

struct JPEGDecoder_ErrorMgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

void JPEGDecoder_ErrorHdlr(j_common_ptr cinfo)
{
	JPEGDecoder_ErrorMgr *myerr = (JPEGDecoder_ErrorMgr*) cinfo->err;

	(*cinfo->err->output_message) (cinfo);
  
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}


Bool Media::JPEGDecoder::Decode(Data::ByteArrayR dataBuff, UnsafeArray<UInt8> imgPtr, UOSInt bpl, UOSInt maxWidth, UOSInt maxHeight, Media::PixelFormat pf) const
{
	const UInt8 *ptr = dataBuff.Ptr();
	JPEGDecoder_ErrorMgr jerr;
	jpeg_decompress_struct cinfo;
	Bool succ = false;
	int ret;
	UInt8 *row = 0;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = JPEGDecoder_ErrorHdlr;
	if (setjmp(jerr.setjmp_buffer)) {
		if (row) MemFree(row);
		jpeg_destroy_decompress(&cinfo);
		return false;
	}
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, ptr, dataBuff.GetSize());
	ret = jpeg_read_header(&cinfo, TRUE);
	printf("image_width = %d, image_height = %d, output_components = %d, out_color_space = %d, ret = %d\r\n", cinfo.image_width, cinfo.image_height, cinfo.output_components, cinfo.out_color_space, ret);
	if (ret == 1 && jpeg_start_decompress(&cinfo))
	{
		if (maxHeight > cinfo.output_height)
		{
			maxHeight = cinfo.output_height;
		}
		if (cinfo.out_color_space == JCS_GRAYSCALE)
		{
			if (pf == Media::PF_PAL_W8)
			{
				if (maxWidth < cinfo.output_width)
				{
					UInt8 *r = imgPtr.Ptr();
					row = MemAlloc(UInt8, cinfo.output_width);
					while (cinfo.output_scanline < maxHeight) {
						jpeg_read_scanlines(&cinfo, &row, 1);
						MemCopyNO(r, row, maxWidth);
						r += bpl;
					}
					MemFree(row);
					row = 0;
				}
				else
				{
					UInt8 *r = imgPtr.Ptr();
					while (cinfo.output_scanline < maxHeight) {
						jpeg_read_scanlines(&cinfo, &r, 1);
						r += bpl;
					}
				}
				succ = true;
			}
		}
		else
		{
	
		}
		jpeg_finish_decompress(&cinfo);
	}
	jpeg_destroy_decompress(&cinfo);
	return succ;
}
