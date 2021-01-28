#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
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

void Parser::FileParser::PCXParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.pcx", (const UTF8Char*)"PCX Image File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::PCXParser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::PCXParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[128];
	UInt8 *dataBuff;
	UInt8 *lineBuff;
//	UInt8 pal[1024];

	Int32 imgWidth;
	Int32 imgHeight;
	Int32 bpp;
	Int32 bppl;
	Int32 bpl;
	Int32 bplp;
	Int32 ncp;

	Media::StaticImage *outImg;

	fd->GetRealData(0, 128, hdr);
	if (hdr[0] != 10 || hdr[2] != 1)
	{
		return 0;
	}

	imgWidth = ReadUInt16(&hdr[8]) - ReadUInt16(&hdr[4]) + 1;
	imgHeight = ReadUInt16(&hdr[10]) - ReadUInt16(&hdr[6]) + 1;
	bppl = hdr[3];
	ncp = hdr[65];
	bpl = ReadUInt16(&hdr[66]);
	bpp = bppl * ncp;
	bplp = bpl * ncp;
//	return 0;

	NEW_CLASS(outImg, Media::StaticImage(imgWidth, imgHeight, 0, bpp, Media::FrameInfo::GetDefPixelFormat(0, bpp), 0, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	if (outImg)
	{
		UInt8 *pBits = (UInt8*)outImg->data;
		OSInt i;
		OSInt j;
		OSInt k;
		UInt8 c;
		OSInt filePos = 128;
		OSInt readSize;
		OSInt dataSize = 0;
		OSInt lineLeft = imgHeight;
		dataBuff = MemAlloc(UInt8, bplp << 1);
		lineBuff = MemAlloc(UInt8, bplp);

		while (lineLeft-- > 0)
		{
			readSize = fd->GetRealData(filePos, (bplp << 1) - dataSize, &dataBuff[dataSize]);
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
				Int32 cnt = imgWidth;
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
				MemCopyO(dataBuff, &dataBuff[i], dataSize - i);
				dataSize -= i;
			}
		}
		MemFree(dataBuff);
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

	if (outImg)
	{
		Media::ImageList *imgList;
		NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
		imgList->AddImage(outImg, 0);
		return imgList;
	}
	else
	{
		return 0;
	}
}
