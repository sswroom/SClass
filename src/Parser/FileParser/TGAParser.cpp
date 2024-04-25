#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "IO/FileParser.h"
#include "Media/IImgResizer.h"
#include "Media/FrameInfo.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/TGAParser.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

Parser::FileParser::TGAParser::TGAParser()
{
}

Parser::FileParser::TGAParser::~TGAParser()
{
}

Int32 Parser::FileParser::TGAParser::GetName()
{
	return *(Int32*)"TGAP";
}

void Parser::FileParser::TGAParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.tga"), CSTR("Targa File"));
	}
}

IO::ParserType Parser::FileParser::TGAParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::TGAParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 footer[26];

//	UInt8 pal[1024];

	UInt32 imgWidth;
	UInt32 imgHeight;
	UInt32 bpp;
	UInt32 imgPos;

	Media::StaticImage *outImg;

	UInt64 ds = fd->GetDataSize();
	if (fd->GetRealData(ds - 26, 26, BYTEARR(footer)) != 26)
	{
		return 0;
	}

	if (*(Int32*)&footer[8] != 0x45555254 || *(Int32*)&footer[12] != 0x49534956 || *(Int32*)&footer[16] != 0x582D4E4F || *(Int32*)&footer[20] != 0x454C4946 || *(Int16*)&footer[24] != 0x2E)
	{
		return 0;
	}

	bpp = hdr[16];
	imgWidth = *(UInt16*)&hdr[12];
	imgHeight = *(UInt16*)&hdr[14];
	if (hdr[17] & 0x10)
	{
		return 0;
	}
	if (bpp != 32)//bpp != 1 && bpp != 2 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32)
	{
		return 0;
	}

	NEW_CLASS(outImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, bpp, Media::PixelFormatGetDef(0, bpp), 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	if (outImg == 0)
	{
		return 0;
	}

	imgPos = 18;
	imgPos += hdr[0];

	if (hdr[1] == 1 && hdr[2] == 1)
	{
		if (hdr[7] == 32)
		{
			DEL_CLASS(outImg);
			return 0;
			/////////////////////////////////////
		}
		else if (hdr[7] == 16 || hdr[7] == 15)
		{
			DEL_CLASS(outImg);
			return 0;
			/////////////////////////////////////
		}
		else if (hdr[7] == 24)
		{
			DEL_CLASS(outImg);
			return 0;
			/////////////////////////////////////
		}
		else
		{
			DEL_CLASS(outImg);
			return 0;
		}
	}
	else if (hdr[1] != 0 || bpp <= 8)
	{
		DEL_CLASS(outImg);
		return 0;
	}

	if (outImg)
	{
		Data::ByteArray pBits = outImg->GetDataArray();
		UInt32 lineW;
		UInt32 lineW2;
		UInt32 currOfst;

		if (hdr[2] == 10)
		{
			Data::ByteBuffer tmpBuff((UOSInt)ds - 26 - imgPos);
			UInt8 *tmpBits = 0;
			fd->GetRealData(imgPos, (UOSInt)ds - 26 - imgPos, tmpBuff);

			if ((hdr[17] & 30) != 20)
			{
				tmpBits = MemAlloc(UInt8, imgWidth * imgHeight * bpp >> 3);
				pBits = Data::ByteArray(tmpBits, imgWidth * imgHeight * bpp >> 3);
			}
			switch (bpp)
			{
#ifdef HAS_ASM32
			case 32:
				_asm
				{
					mov edi,pBits
					mov esi,tmpBuff
					mov eax,imgHeight
					mul imgWidth
					mov ebx,eax

rle32lop:
					movzx eax,byte ptr [esi]
					test al,0x80
					jz rle32lop2
					and eax,0x7f
					inc eax
					mov ecx,eax
					mov eax,dword ptr [esi+1]
					add esi,5
					mov edx,ecx
					rep stosd
					sub ebx,edx
					jz rle32exit
					js rle32exit
					jmp rle32lop
rle32lop2:
					inc esi
					inc eax
					mov ecx,eax
					rep movsd
					sub ebx,eax
					jz rle32exit
					js rle32exit
					jmp rle32lop
rle32exit:
				}
				break;
#else
			case 32:
				{
					UInt8 *tmpSPtr;
					UInt8 *tmpDPtr;
					UOSInt cnt = imgHeight * imgWidth;
					UInt32 tmpVal;
					tmpSPtr = tmpBuff.GetPtr();
					tmpDPtr = pBits.GetPtr();
					while (cnt > 0)
					{
						tmpVal = *tmpSPtr;
						if (0x80 & tmpVal)
						{
							tmpVal = 1 + (tmpVal & 0x7f);
							cnt -= tmpVal;
							tmpSPtr++;
							while (tmpVal-- > 0)
							{
								*(Int32*)tmpDPtr = *(Int32*)tmpSPtr;
								tmpDPtr += 4;
							}
							tmpSPtr += 4;
						}
						else
						{
							tmpVal = 1 + tmpVal;
							tmpSPtr++;
							cnt -= tmpVal;
							while (tmpVal-- > 0)
							{
								*(Int32*)tmpDPtr = *(Int32*)tmpSPtr;
								tmpDPtr += 4;
								tmpSPtr += 4;
							}
						}
					}
				}
#endif
			default:
				DEL_CLASS(outImg);
				outImg = 0;
				break;
			}
			if ((hdr[17] & 30) != 20)
			{
				if (outImg)
				{
					UInt32 bpl = imgWidth * bpp >> 3;
					UInt8 *tmpOfst = tmpBits + imgHeight * bpl;
					pBits = outImg->GetDataArray();
					while (imgHeight-- > 0)
					{
						tmpOfst -= bpl;
						pBits.CopyFrom(0, Data::ByteArrayR(tmpOfst, bpl));
						pBits += bpl;
					}
				}
				MemFree(tmpBits);
			}
		}
		else if (hdr[17] & 0x20)
		{
			switch (bpp)
			{
			case 4:
				lineW = (imgWidth >> 1) + (imgWidth & 1);
				lineW2 = lineW;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				currOfst = imgPos;
				while (imgHeight-- > 0)
				{
					fd->GetRealData(currOfst, lineW2, pBits);
					currOfst += lineW;
					pBits += lineW2;
				}
				break;
			case 8:
			case 16:
			case 24:
			case 32:
				fd->GetRealData(imgPos, imgWidth * imgHeight * bpp >> 3, pBits);
				break;
			default:
				DEL_CLASS(outImg);
				outImg = 0;
				break;
			}
		}
		else
		{
			switch (bpp)
			{
			case 4:
				lineW = (imgWidth >> 1) + (imgWidth & 1);
				lineW2 = lineW;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				currOfst = (lineW * imgHeight) + imgPos;
				while (imgHeight-- > 0)
				{
					currOfst -= lineW;
					fd->GetRealData(currOfst, lineW2, pBits);
					pBits += lineW2;
				}
				break;
			case 8:
				lineW = imgWidth;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				currOfst = (lineW * imgHeight) + imgPos;
				while (imgHeight-- > 0)
				{
					currOfst -= lineW;
					fd->GetRealData(currOfst, imgWidth, pBits);
					pBits += imgWidth;
				}
				break;
			case 16:
				lineW = imgWidth;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				lineW2 = imgWidth << 1;
				currOfst = (lineW * imgHeight) + imgPos;
				while (imgHeight-- > 0)
				{
					currOfst -= lineW;
					fd->GetRealData(currOfst, lineW2, pBits);
					pBits += lineW2;
				}
				break;
			case 24:
				lineW = imgWidth * 3;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				lineW2 = imgWidth * 3;
				currOfst = (lineW * imgHeight) + imgPos;
				while (imgHeight-- > 0)
				{
					currOfst -= lineW;
					fd->GetRealData(currOfst, lineW2, pBits);
					pBits += lineW2;
				}
				break;
			case 32:
				currOfst = imgPos;
				pBits = pBits + imgWidth * imgHeight * 4;
				while (imgHeight-- > 0)
				{
					pBits -= imgWidth << 2;
					fd->GetRealData(currOfst, imgWidth << 2, pBits);
					currOfst += imgWidth << 2;
				}
				break;
			default:
				DEL_CLASS(outImg);
				outImg = 0;
				break;
			};
		}
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
