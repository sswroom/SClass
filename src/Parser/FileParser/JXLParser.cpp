#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/JXLParser.h"

#include <jxl/decode.h>
#include <jxl/decode_cxx.h>
#include <jxl/resizable_parallel_runner.h>
#include <jxl/resizable_parallel_runner_cxx.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
Parser::FileParser::JXLParser::JXLParser()
{
}

Parser::FileParser::JXLParser::~JXLParser()
{
}

Int32 Parser::FileParser::JXLParser::GetName()
{
	return *(Int32*)"JXLP";
}

void Parser::FileParser::JXLParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.JXL"), CSTR("JPEG XL File"));
	}
}

IO::ParserType Parser::FileParser::JXLParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::JXLParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr.ReadMU32(0) != 12 || *(Int32*)&hdr[4] != *(Int32*)"JXL " || ReadMUInt32(&hdr[8]) != 0x0d0a870a)
	{
		return 0;
	}
	JxlResizableParallelRunnerPtr runner = JxlResizableParallelRunnerMake(nullptr);
	JxlDecoderPtr dec = JxlDecoderMake(nullptr);
	if (JXL_DEC_SUCCESS != JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO |
												JXL_DEC_COLOR_ENCODING |
												JXL_DEC_FULL_IMAGE)) {
		return 0;
	}

	if (JXL_DEC_SUCCESS != JxlDecoderSetParallelRunner(dec.get(), JxlResizableParallelRunner, runner.get())) {
		return 0;
	}

	JxlBasicInfo info;
	JxlPixelFormat format = {4, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
	UInt64 dataSize = fd->GetDataSize();
	Data::ByteBuffer buff((UOSInt)dataSize);
	if (fd->GetRealData(0, buff.GetSize(), buff) != buff.GetSize())
	{
		return 0;
	}
	JxlDecoderSetInput(dec.get(), buff.Ptr(), buff.GetSize());
	JxlDecoderCloseInput(dec.get());

	Media::FrameInfo finfo;
	Optional<Media::StaticImage> optimg = 0;
	NN<Media::StaticImage> simg;
	finfo.Clear();
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(fd->GetFullName()));
	for (;;) {
		JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());

		if (status == JXL_DEC_ERROR) {
			printf("JXLParser: Decoder error\r\n");
			optimg.Delete();
			imgList.Delete();
			return 0;
		} else if (status == JXL_DEC_NEED_MORE_INPUT) {
			printf("JXLParser: Need More input\r\n");
			optimg.Delete();
			imgList.Delete();
			return 0;
		} else if (status == JXL_DEC_BASIC_INFO) {
			if (JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &info)) {
				printf("JXLParser: JxlDecoderGetBasicInfo failed\r\n");
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
			optimg.Delete();
//			printf("Basic Info: %d x %d, bpp = %d, ebps = %d\r\n", info.xsize, info.ysize, info.bits_per_sample, info.exponent_bits_per_sample);
			finfo.dispSize = Math::Size2D<UOSInt>(info.xsize, info.ysize);
			if (info.exponent_bits_per_sample == 0)
			{
				if (info.bits_per_sample == 16)
				{
					if (info.num_color_channels == 1)
					{
						if (info.alpha_bits > 0)
						{
							finfo.atype = Media::AT_ALPHA;
							finfo.storeBPP = 32;
							finfo.pf = Media::PF_LE_W16A16;
							format = {2, JXL_TYPE_UINT16, JXL_NATIVE_ENDIAN, 0};
						}
						else
						{
							finfo.atype = Media::AT_NO_ALPHA;
							finfo.storeBPP = 16;
							finfo.pf = Media::PF_LE_W16;
							format = {1, JXL_TYPE_UINT16, JXL_NATIVE_ENDIAN, 0};
						}
					}
					else
					{
						if (info.alpha_bits > 0)
						{
							finfo.atype = Media::AT_ALPHA;
							finfo.storeBPP = 64;
							finfo.pf = Media::PF_LE_R16G16B16A16;
							format = {4, JXL_TYPE_UINT16, JXL_NATIVE_ENDIAN, 0};
						}
						else
						{
							finfo.atype = Media::AT_NO_ALPHA;
							finfo.storeBPP = 48;
							finfo.pf = Media::PF_LE_R16G16B16;
							format = {3, JXL_TYPE_UINT16, JXL_NATIVE_ENDIAN, 0};
						}
					}
				}
				else
				{
					if (info.num_color_channels == 1)
					{
						if (info.alpha_bits > 0)
						{
							finfo.atype = Media::AT_ALPHA;
							finfo.storeBPP = 16;
							finfo.pf = Media::PF_W8A8;
							format = {2, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
						}
						else
						{
							finfo.atype = Media::AT_NO_ALPHA;
							finfo.storeBPP = 8;
							finfo.pf = Media::PF_PAL_W8;
							format = {1, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
						}
					}
					else
					{
						if (info.alpha_bits > 0)
						{
							finfo.atype = Media::AT_ALPHA;
							finfo.storeBPP = 32;
							finfo.pf = Media::PF_R8G8B8A8;
							format = {4, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
						}
						else
						{
							finfo.atype = Media::AT_NO_ALPHA;
							finfo.storeBPP = 24;
							finfo.pf = Media::PF_R8G8B8;
							format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
						}
					}
				}
			}
			else
			{
				if (info.num_color_channels == 1)
				{
					if (info.alpha_bits > 0)
					{
						finfo.atype = Media::AT_ALPHA;
						finfo.storeBPP = 64;
						finfo.pf = Media::PF_LE_FW32A32;
						format = {2, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
					}
					else
					{
						finfo.atype = Media::AT_NO_ALPHA;
						finfo.storeBPP = 32;
						finfo.pf = Media::PF_LE_FW32;
						format = {1, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
					}
				}
				else
				{
					if (info.alpha_bits > 0)
					{
						finfo.atype = Media::AT_ALPHA;
						finfo.storeBPP = 128;
						finfo.pf = Media::PF_LE_FR32G32B32A32;
						format = {4, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
					}
					else
					{
						finfo.atype = Media::AT_NO_ALPHA;
						finfo.storeBPP = 96;
						finfo.pf = Media::PF_LE_FR32G32B32;
						format = {3, JXL_TYPE_FLOAT, JXL_NATIVE_ENDIAN, 0};
					}
				}
			}
			NEW_CLASSNN(simg, Media::StaticImage(finfo));
			optimg = simg;
			JxlResizableParallelRunnerSetThreads(runner.get(), JxlResizableParallelRunnerSuggestThreads(info.xsize, info.ysize));
		} else if (status == JXL_DEC_COLOR_ENCODING) {
			size_t icc_size;
			if (JXL_DEC_SUCCESS != JxlDecoderGetICCProfileSize(dec.get(), JXL_COLOR_PROFILE_TARGET_DATA, &icc_size)) {
				printf("JXLParser: JxlDecoderGetICCProfileSize failed\r\n");
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
			if (!optimg.SetTo(simg))
			{
				printf("JXLParser: Image Not found on Color Encoding\r\n");
				imgList.Delete();
				return 0;
			}
			UInt8 *iccBuff = MemAlloc(UInt8, icc_size);
			if (JXL_DEC_SUCCESS != JxlDecoderGetColorAsICCProfile(dec.get(), JXL_COLOR_PROFILE_TARGET_DATA, iccBuff, icc_size)) {
				printf("JXLParser: JxlDecoderGetColorAsICCProfile failed\r\n");
				optimg.Delete();
				MemFree(iccBuff);
				imgList.Delete();
				return 0;
			}
			NN<Media::ICCProfile> icc;
			if (Media::ICCProfile::Parse(Data::ByteArrayR(iccBuff, icc_size)).SetTo(icc))
			{
				icc->SetToColorProfile(simg->info.color);
			}
			MemFree(iccBuff);
		} else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
			size_t buffer_size;
			if (JXL_DEC_SUCCESS != JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size)) {
				printf("JXLParser: JxlDecoderImageOutBufferSize failed\r\n");
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
			if (!optimg.SetTo(simg))
			{
				printf("JXLParser: Image Not found\r\n");
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
			if (buffer_size > simg->GetDataBpl() * simg->info.dispSize.y) {
				printf("JXLParser: Invalid out buffer size %d, %d x %d x %d\r\n", (UInt32)buffer_size, (UInt32)simg->info.dispSize.x, (UInt32)simg->info.dispSize.y, (UInt32)simg->info.storeBPP);
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
			if (JXL_DEC_SUCCESS != JxlDecoderSetImageOutBuffer(dec.get(), &format,
																simg->data.Ptr(),
																buffer_size)) {
				printf("JXLParser: JxlDecoderSetImageOutBuffer failed\r\n");
				optimg.Delete();
				imgList.Delete();
				return 0;
			}
		} else if (status == JXL_DEC_FULL_IMAGE) {
			if (optimg.SetTo(simg))
			{
				imgList->AddImage(simg, 0);
				optimg = 0;
			}
			else
			{
				printf("JXLParser: Image not found on Full Image\r\n");
			}
		} else if (status == JXL_DEC_SUCCESS) {
			optimg.Delete();
			if (imgList->GetCount() == 0)
			{
				printf("JXLParser: Image not found on Success\r\n");
				imgList.Delete();
				return 0;
			}
			return imgList;
		} else {
			printf("JXLParser: Unknown Status: %d\r\n", status);
			optimg.Delete();
			imgList.Delete();
			return 0;
		}
	}

	return 0;
}
