#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Media/ICCProfile.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/WebPParser.h"

#include <webp/decode.h>
#include <webp/demux.h>

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif
Parser::FileParser::WebPParser::WebPParser()
{
}

Parser::FileParser::WebPParser::~WebPParser()
{
}

Int32 Parser::FileParser::WebPParser::GetName()
{
	return *(Int32*)"WEBP";
}

void Parser::FileParser::WebPParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.webp"), CSTR("WebP"));
	}
}

IO::ParserType Parser::FileParser::WebPParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::WebPParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt64 fileLen = fd->GetDataSize();
	if (fileLen < 32 || fileLen > 104857600)
		return 0;
	if (ReadNInt32(&hdr[0]) != *(Int32*)"RIFF" || ReadUInt32(&hdr[4]) != (fileLen - 8) || ReadNInt32(&hdr[8]) != *(Int32*)"WEBP")
		return 0;

	Data::ByteBuffer fileData((UOSInt)fileLen);
	if (fd->GetRealData(0, (UOSInt)fileLen, fileData) != fileLen)
	{
		return 0;
	}
	WebPData data;
	data.bytes = fileData.Ptr();
	data.size = (size_t)fileLen;
	WebPDemuxer* demux = WebPDemux(&data);
	if (demux == 0)
	{
		return 0;
	}

	UInt32 width = WebPDemuxGetI(demux, WEBP_FF_CANVAS_WIDTH);
	UInt32 height = WebPDemuxGetI(demux, WEBP_FF_CANVAS_HEIGHT);
	UInt32 flags = WebPDemuxGetI(demux, WEBP_FF_FORMAT_FLAGS);

	Media::StaticImage *simg;
	Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
	NEW_CLASS(simg, Media::StaticImage(Math::Size2D<UOSInt>(width, height), 0, 32, Media::PF_B8G8R8A8, width * height * 4, &color, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT))
	WebPIterator iter;
	if (WebPDemuxGetFrame(demux, 1, &iter))
	{
		do
		{
			WebPDecodeBGRAInto(iter.fragment.bytes, iter.fragment.size, simg->data, simg->GetDataBpl() * simg->info.dispSize.y, (int)simg->GetDataBpl());
		} while (WebPDemuxNextFrame(&iter));
		WebPDemuxReleaseIterator(&iter);
	}

	// ... (Extract metadata).
	WebPChunkIterator chunk_iter;
	if (flags & ICCP_FLAG)
	{
 		WebPDemuxGetChunk(demux, "ICCP", 1, &chunk_iter);
		Media::ICCProfile *profile = Media::ICCProfile::Parse(Data::ByteArrayR(chunk_iter.chunk.bytes, (UOSInt)chunk_iter.chunk.size));
		if (profile)
		{
			profile->SetToColorProfile(simg->info.color);
			DEL_CLASS(profile);
		}
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	if (flags & EXIF_FLAG)
	{
		WebPDemuxGetChunk(demux, "EXIF", 1, &chunk_iter);
		const UInt8 *buff = chunk_iter.chunk.bytes;
		Bool succ = true;
		Data::ByteOrder *bo = 0;
		if (*(Int16*)&buff[0] == *(Int16*)"II")
		{
			NEW_CLASS(bo, Data::ByteOrderLSB());
		}
		else if (*(Int16*)&buff[0] == *(Int16*)"MM")
		{
			NEW_CLASS(bo, Data::ByteOrderMSB());
		}
		else
		{
			succ = false;
		}
		if (succ)
		{
			if (bo->GetUInt16(&buff[2]) != 42)
			{
				succ = false;
			}
			if (bo->GetUInt32(&buff[4]) != 8)
			{
				succ = false;
			}
		}
		if (succ)
		{
			if (simg->exif)
			{
				DEL_CLASS(simg->exif);
			}
			UInt64 nextOfst;
			simg->exif = Media::EXIFData::ParseIFD(buff + 8, chunk_iter.chunk.size - 8, bo, &nextOfst, Media::EXIFData::EM_STANDARD, buff);
		}
		SDEL_CLASS(bo);
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	if (flags & XMP_FLAG)
	{
		WebPDemuxGetChunk(demux, "XMP ", 1, &chunk_iter);
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	WebPDemuxDelete(demux);
	
	if (simg->exif)
	{
		Double hdpi = simg->exif->GetHDPI();
		Double vdpi = simg->exif->GetVDPI();
		if (hdpi != 0 && vdpi != 0)
		{
			simg->info.hdpi = hdpi;
			simg->info.vdpi = vdpi;
		}
	}

	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullFileName()));
	imgList->AddImage(simg, 0);
	return imgList;
}
