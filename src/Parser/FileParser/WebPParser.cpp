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

void Parser::FileParser::WebPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::WebPParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
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

	NN<Media::StaticImage> simg;
	NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UOSInt>(width, height), 0, 32, Media::PF_B8G8R8A8, width * height * 4, Media::ColorProfile(Media::ColorProfile::CPT_PUNKNOWN), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT))
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
		NN<Media::ICCProfile> profile;
		if (Media::ICCProfile::Parse(Data::ByteArrayR(chunk_iter.chunk.bytes, (UOSInt)chunk_iter.chunk.size)).SetTo(profile))
		{
			profile->SetToColorProfile(simg->info.color);
			profile.Delete();
		}
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	if (flags & EXIF_FLAG)
	{
		WebPDemuxGetChunk(demux, "EXIF", 1, &chunk_iter);
		const UInt8 *buff = chunk_iter.chunk.bytes;
		Bool succ = true;
		NN<Data::ByteOrder> bo;
		if (*(Int16*)&buff[0] == *(Int16*)"II")
		{
			NEW_CLASSNN(bo, Data::ByteOrderLSB());
		}
		else if (*(Int16*)&buff[0] == *(Int16*)"MM")
		{
			NEW_CLASSNN(bo, Data::ByteOrderMSB());
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
			if (succ)
			{
				simg->exif.Delete();
				UInt64 nextOfst;
				simg->exif = Media::EXIFData::ParseIFD(buff + 8, chunk_iter.chunk.size - 8, bo, nextOfst, Media::EXIFData::EM_STANDARD, buff);
			}
			bo.Delete();
		}
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	if (flags & XMP_FLAG)
	{
		WebPDemuxGetChunk(demux, "XMP ", 1, &chunk_iter);
		WebPDemuxReleaseChunkIterator(&chunk_iter);
	}
	WebPDemuxDelete(demux);
	
	NN<Media::EXIFData> exif;
	if (simg->exif.SetTo(exif))
	{
		Double hdpi = exif->GetHDPI();
		Double vdpi = exif->GetVDPI();
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
