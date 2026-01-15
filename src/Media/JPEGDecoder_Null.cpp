#include "Stdafx.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageList.h"
#include "Media/JPEGDecoder.h"
#include "Parser/FileParser/GUIImgParser.h"

Media::JPEGDecoder::JPEGDecoder()
{
}

Media::JPEGDecoder::~JPEGDecoder()
{
}

Bool Media::JPEGDecoder::Decode(Data::ByteArrayR dataBuff, UnsafeArray<UInt8> imgPtr, UIntOS bpl, UIntOS maxWidth, UIntOS maxHeight, Media::PixelFormat pf) const
{
	Parser::FileParser::GUIImgParser parser;
	NN<Media::ImageList> imgList;
	IO::StmData::MemoryDataRef md(dataBuff);
	if (Optional<Media::ImageList>::ConvertFrom(parser.ParseFileHdr(md, 0, IO::ParserType::ImageList, dataBuff)).SetTo(imgList))
	{
		NN<Media::StaticImage> simg;
		imgList->ToStaticImage(0);
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(simg))
		{
			Bool succ = false;
			if (pf == simg->info.pf)
			{
				succ = true;
				if (simg->info.dispSize.GetWidth() < maxWidth)
				{
					maxWidth = simg->info.dispSize.GetWidth();
				}
				if (simg->info.dispSize.GetHeight() < maxHeight)
				{
					maxHeight = simg->info.dispSize.GetHeight();
				}
				ImageCopy_ImgCopy(simg->data.Ptr(), imgPtr.Ptr(), (maxWidth * simg->info.storeBPP + 7) >> 3, maxHeight, simg->GetDataBpl(), bpl);
			}
			else
			{
				printf("JPEGDecoder_Null: pf not supported: %s != %s\r\n", Media::PixelFormatGetName(pf).v.Ptr(), Media::PixelFormatGetName(simg->info.pf).v.Ptr());
			}
			imgList.Delete();
			return succ;
		}
		imgList.Delete();
	}
	return false;
}

Optional<Media::StaticImage> Media::JPEGDecoder::DecodeImage(Data::ByteArrayR dataBuff) const
{
	Parser::FileParser::GUIImgParser parser;
	NN<Media::ImageList> imgList;
	IO::StmData::MemoryDataRef md(dataBuff);
	if (Optional<Media::ImageList>::ConvertFrom(parser.ParseFileHdr(md, 0, IO::ParserType::ImageList, dataBuff)).SetTo(imgList))
	{
		NN<Media::StaticImage> simg;
		imgList->ToStaticImage(0);
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(simg))
		{
			imgList->RemoveImage(0, false);
			imgList.Delete();
			return simg;
		}
		imgList.Delete();
	}
	return 0;
}

