#include "Stdafx.h"
#include "Media/JPEGDecoder.h"
#include <stddef.h>
#include <jpeglib.h>
#include <setjmp.h>

//#define VERBOSE

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
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
	jpeg_mem_src(&cinfo, ptr, dataBuff.GetSize());
#else
	jpeg_mem_src(&cinfo, (unsigned char*)ptr, dataBuff.GetSize());
#endif
	ret = jpeg_read_header(&cinfo, TRUE);
#if defined(VERBOSE)
	printf("image_width = %d, image_height = %d, output_components = %d, out_color_space = %d, ret = %d\r\n", cinfo.image_width, cinfo.image_height, cinfo.output_components, cinfo.jpeg_color_space, ret);
#endif
	if (cinfo.jpeg_color_space == JCS_RGB)
	{
		if (pf == Media::PF_B8G8R8)
		{
			cinfo.out_color_space = JCS_EXT_BGR;
		}
	}
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
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
					if (cinfo.output_scanline < cinfo.image_height)
					{
						jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
					}
#else
					while (cinfo.output_scanline < cinfo.image_height)
					{
						jpeg_read_scanlines(&cinfo, &row, 1);
					}
#endif
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
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
					if (cinfo.output_scanline < cinfo.image_height)
					{
						jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
					}
#else
					row = MemAlloc(UInt8, cinfo.output_width);
					while (cinfo.output_scanline < cinfo.image_height)
					{
						jpeg_read_scanlines(&cinfo, &row, 1);
					}
					MemFree(row);
					row = 0;
#endif
				}
				succ = true;
			}
		}
		else if (cinfo.jpeg_color_space == JCS_RGB)
		{
			if (cinfo.out_color_components == 3)
			{
				if (pf == Media::PF_B8G8R8 || pf == Media::PF_R8G8B8)
				{
					if (maxWidth < cinfo.output_width)
					{
						UInt8 *r = imgPtr.Ptr();
						row = MemAlloc(UInt8, cinfo.output_width * 3);
						while (cinfo.output_scanline < maxHeight) {
							jpeg_read_scanlines(&cinfo, &row, 1);
							MemCopyNO(r, row, maxWidth * 3);
							r += bpl;
						}
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
						if (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
						}
#else
						while (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_read_scanlines(&cinfo, &row, 1);
						}
#endif
						MemFree(row);
						row = 0;
						succ = true;
					}
					else
					{
						UInt8 *r = imgPtr.Ptr();
						while (cinfo.output_scanline < maxHeight) {
							jpeg_read_scanlines(&cinfo, &r, 1);
							r += bpl;
						}
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
						if (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
						}
#else
						row = MemAlloc(UInt8, cinfo.output_width * 3);
						while (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_read_scanlines(&cinfo, &row, 1);
						}
						MemFree(row);
						row = 0;
#endif
						succ = true;
					}
				}
			}
		}
		else if (cinfo.jpeg_color_space == JCS_YCbCr)
		{
			if (cinfo.out_color_components == 3)
			{
				if (pf == Media::PF_B8G8R8 || pf == Media::PF_R8G8B8)
				{
					if (maxWidth < cinfo.output_width)
					{
						UInt8 *r = imgPtr.Ptr();
						row = MemAlloc(UInt8, cinfo.output_width * 3);
						while (cinfo.output_scanline < maxHeight) {
							jpeg_read_scanlines(&cinfo, &row, 1);
							MemCopyNO(r, row, maxWidth * 3);
							r += bpl;
						}
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
						if (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
						}
#else
						while (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_read_scanlines(&cinfo, &row, 1);
						}
#endif
						MemFree(row);
						row = 0;
						succ = true;
					}
					else
					{
						UInt8 *r = imgPtr.Ptr();
						while (cinfo.output_scanline < maxHeight) {
							jpeg_read_scanlines(&cinfo, &r, 1);
							r += bpl;
						}
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
						if (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_skip_scanlines(&cinfo, cinfo.image_height - cinfo.output_scanline);
						}
#else
						row = MemAlloc(UInt8, cinfo.output_width * 3);
						while (cinfo.output_scanline < cinfo.image_height)
						{
							jpeg_read_scanlines(&cinfo, &row, 1);
						}
						MemFree(row);
						row = 0;
#endif
						succ = true;
					}
				}
			}
		}
		if (!succ)
		{
			printf("JPEGDecoder: Pixel format not supported: img pf = %s, jpg cs = %d, ch = %d\r\n", Media::PixelFormatGetName(pf).v.Ptr(), cinfo.jpeg_color_space, cinfo.out_color_components);
		}
		jpeg_finish_decompress(&cinfo);
	}
	jpeg_destroy_decompress(&cinfo);
	return succ;
}

Optional<Media::StaticImage> Media::JPEGDecoder::DecodeImage(Data::ByteArrayR dataBuff) const
{
	const UInt8 *ptr = dataBuff.Ptr();
	JPEGDecoder_ErrorMgr jerr;
	jpeg_decompress_struct cinfo;
	Optional<Media::StaticImage> img = nullptr;
	NN<Media::StaticImage> nnimg;
	int ret;
	UInt8 *row = 0;
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = JPEGDecoder_ErrorHdlr;
	if (setjmp(jerr.setjmp_buffer)) {
		if (row) MemFree(row);
		img.Delete();
		jpeg_destroy_decompress(&cinfo);
		return nullptr;
	}
	jpeg_create_decompress(&cinfo);
#ifdef LIBJPEG_TURBO_VERSION_NUMBER
	jpeg_mem_src(&cinfo, ptr, dataBuff.GetSize());
#else
	jpeg_mem_src(&cinfo, (unsigned char*)ptr, dataBuff.GetSize());
#endif
	ret = jpeg_read_header(&cinfo, TRUE);
#if defined(VERBOSE)
	printf("image_width = %d, image_height = %d, output_components = %d, out_color_space = %d, ret = %d\r\n", cinfo.image_width, cinfo.image_height, cinfo.output_components, cinfo.out_color_space, ret);
#endif
	if (ret == 1 && jpeg_start_decompress(&cinfo))
	{
		if (cinfo.out_color_space == JCS_GRAYSCALE)
		{
			Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
			NEW_CLASSNN(nnimg, Media::StaticImage(Math::Size2D<UOSInt>(cinfo.image_width, cinfo.image_height), 0, 8, Media::PixelFormat::PF_PAL_W8, 0, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));
			img = nnimg;
			nnimg->InitGrayPal();
			UOSInt bpl = nnimg->GetDataBpl();
			UInt8 *r = nnimg->data.Ptr();
			while (cinfo.output_scanline < cinfo.image_height) {
				jpeg_read_scanlines(&cinfo, &r, 1);
				r += bpl;
			}
		}
		else
		{
			Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
			NEW_CLASSNN(nnimg, Media::StaticImage(Math::Size2D<UOSInt>(cinfo.image_width, cinfo.image_height), 0, 24, Media::PixelFormat::PF_R8G8B8, 0, color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA_ALL_FF, Media::YCOFST_C_CENTER_LEFT));
			img = nnimg;
			UOSInt bpl = nnimg->GetDataBpl();
			UInt8 *r = nnimg->data.Ptr();
			while (cinfo.output_scanline < cinfo.image_height) {
				jpeg_read_scanlines(&cinfo, &r, 1);
				r += bpl;
			}
		}
		if (img.IsNull())
		{
			printf("JPEGDecoder: Pixel format not supported: jpg cs = %d, ch = %d\r\n", cinfo.out_color_space, cinfo.out_color_components);
		}
		jpeg_finish_decompress(&cinfo);
	}
	jpeg_destroy_decompress(&cinfo);
	return img;
}
