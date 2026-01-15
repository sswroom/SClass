#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Media/FrameInfo.h"
#include "Media/ImageResizer.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/PCXParser.h"

Parser::FileParser::PCXParser::PCXParser()
{
}

Parser::FileParser::PCXParser::~PCXParser()
{
}

Int32 Parser::FileParser::PCXParser::GetName()
{
	return *(Int32*)"PCXP";
}

void Parser::FileParser::PCXParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.pcx"), CSTR("PCX Image File"));
	}
}

IO::ParserType Parser::FileParser::PCXParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::PCXParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 *lineBuff;
//	UInt8 pal[1024];

	UInt32 imgWidth;
	UInt32 imgHeight;
	UInt32 bpp;
	UInt32 bppl;
	UInt32 bpl;
	UInt32 bplp;
	UInt32 ncp;

	Media::StaticImage *outImg;
	NN<Media::StaticImage> nnimg;

	if (hdr[0] != 10 || hdr[2] != 1)
	{
		return nullptr;
	}

	imgWidth = ReadUInt16(&hdr[8]) - (UInt32)ReadUInt16(&hdr[4]) + 1;
	imgHeight = ReadUInt16(&hdr[10]) - (UInt32)ReadUInt16(&hdr[6]) + 1;
	bppl = hdr[3];
	ncp = hdr[65];
	bpl = ReadUInt16(&hdr[66]);
	bpp = bppl * ncp;
	bplp = bpl * ncp;
//	return 0;

	NEW_CLASS(outImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, bpp, Media::PixelFormatGetDef(0, bpp), 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_IGNORE_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	if (outImg)
	{
		UInt8 *pBits = outImg->data.Ptr();
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UInt8 c;
		UOSInt filePos = 128;
		UOSInt readSize;
		UOSInt dataSize = 0;
		UOSInt lineLeft = imgHeight;
		Data::ByteBuffer dataBuff(bplp << 1);
		lineBuff = MemAlloc(UInt8, bplp);

		while (lineLeft-- > 0)
		{
			readSize = fd->GetRealData(filePos, (bplp << 1) - dataSize, dataBuff.SubArray(dataSize));
			dataSize += readSize;
			filePos += readSize;

			i = 0;
			j = 0;
			while (j < bplp)
			{
				c = dataBuff[i];
				if (c >= 0xc0)
				{
					k = c & 0x3f;
					c = dataBuff[i + 1];
					i += 2;
					while (k-- > 0)
					{
						lineBuff[j++] = c;
					}
				}
				else
				{
					lineBuff[j++] = c;
					i++;
				}
			}
			if (bppl == 8 && ncp == 3)
			{
#ifdef HAS_ASM32
				_asm
				{
					mov edi,pBits
					mov ecx,imgWidth
					mov esi,lineBuff
					mov ebx,bpl
lop24:
					mov al,byte ptr [esi]
					mov byte ptr [edi+2],al
					mov al,byte ptr [esi + ebx]
					mov byte ptr [edi+1],al
					mov al,byte ptr [esi + ebx * 2]
					mov byte ptr [edi],al
					inc esi
					add edi,3
					dec ecx
					jnz lop24
				}
#else
				UInt8 *dPtr = pBits;
				UInt8 *sPtr = lineBuff;
				UInt32 cnt = imgWidth;
				while (cnt-- > 0)
				{
					dPtr[2] = sPtr[0];
					dPtr[1] = sPtr[bpl];
					dPtr[0] = sPtr[bpl * 2];
					sPtr++;
					dPtr += 3;
				}
#endif
			}
			else
			{
				MemCopyNO(pBits, lineBuff, (imgWidth * bpp) >> 3);
			}
			pBits += (imgWidth * bpp) >> 3;
			if (i >= dataSize)
			{
				dataSize = 0;
			}
			else
			{
				dataBuff.CopyInner(0, i, dataSize - i);
				dataSize -= i;
			}
		}
		MemFree(lineBuff);

/*		switch (bpp)
		{
		case 4:
     		fd->GetRealData(endPos, 64, outImg->pal);
			i = 16;
			while (i-- > 0)
			{
				outImg->pal[(i << 2) + 3] = 0xff;
			}
			lineW = (imgWidth >> 1) + (imgWidth & 1);
			lineW2 = lineW;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * imgHeight) + *(Int32*)&hdr[10];
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				fd->GetRealData(currOfst, lineW2, pBits);
				pBits += lineW2;
			}
			break;
		};*/
	}

	if (nnimg.Set(outImg))
	{
		Media::ImageList *imgList;
		NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
		imgList->AddImage(nnimg, 0);
		return imgList;
	}
	else
	{
		return nullptr;
	}
}
