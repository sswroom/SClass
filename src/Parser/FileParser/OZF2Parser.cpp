#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "Media/FrameInfo.h"
#include "Media/ImageResizer.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/OZF2Parser.h"

Parser::FileParser::OZF2Parser::OZF2Parser()
{
}

Parser::FileParser::OZF2Parser::~OZF2Parser()
{
}

Int32 Parser::FileParser::OZF2Parser::GetName()
{
	return *(Int32*)"OZF2";
}

void Parser::FileParser::OZF2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.ozf2"), CSTR("OziExplorer File v2"));
	}
}

IO::ParserType Parser::FileParser::OZF2Parser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::OZF2Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 tmpBuff[1036];
	UOSInt i;
	UOSInt j;
	UOSInt currX;
	UOSInt currY;
	OSInt imgX1;
	OSInt imgY1;
	OSInt imgX2;
	OSInt imgY2;

	UInt32 imgWidth;
	UInt32 imgHeight;
	UInt32 scaleCnt;
	UInt64 fileSize;
	
	Media::ImageList *imgList;
	NN<Media::StaticImage> outImg;

	fileSize = fd->GetDataSize();
	if (fileSize > 0xffffffffLL)
	{
		return 0;
	}
	if (ReadInt16(&hdr[0]) != 0x7778)
	{
		return 0;
	}
	if (ReadInt16(&hdr[6]) != 64)
	{
		return 0;
	}
	if (ReadInt16(&hdr[8]) != 1)
	{
		return 0;
	}
	if (ReadInt32(&hdr[14]) != 40)
	{
		return 0;
	}
	imgWidth = ReadUInt32(&hdr[18]);
	imgHeight = ReadUInt32(&hdr[22]);
	scaleCnt = ReadUInt16(&hdr[58]);
	imgList = 0;
	fd->GetRealData(fileSize - 4, 4, BYTEARR(tmpBuff));
	scaleCnt = (UInt32)((fileSize - 4 - ReadUInt32(tmpBuff)) >> 2);

	Data::ByteBuffer imgBuff(4096);
	Data::ByteBuffer srcBuff(8192);
	Data::Compress::Inflate inf(false);
	Data::ByteBuffer scaleTable((scaleCnt + 1) * 4);
	fd->GetRealData(ReadUInt32(tmpBuff), scaleCnt * 4, scaleTable);
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullFileName()));
	i = 0;
	while (i < scaleCnt)
	{
		UInt32 thisImgWidth;
		UInt32 thisImgHeight;
		UInt32 thisOfst;
		UInt32 nextOfst;
		UOSInt xCnt;
		UOSInt yCnt;
		UOSInt decSize;
		fd->GetRealData(ReadUInt32(&scaleTable[i * 4]), 1036, BYTEARR(tmpBuff));
		thisImgWidth = ReadUInt32(&tmpBuff[0]);
		thisImgHeight = ReadUInt32(&tmpBuff[4]);
		xCnt = ReadUInt16(&tmpBuff[8]);
		yCnt = ReadUInt16(&tmpBuff[10]);
		if (thisImgWidth == imgWidth && thisImgHeight == imgHeight)
		{
			NEW_CLASSNN(outImg, Media::StaticImage(Math::Size2D<UOSInt>(thisImgWidth, thisImgHeight), 0, 8, Media::PF_PAL_8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			MemCopyNO(outImg->pal, &tmpBuff[12], 1024);

			Data::ByteBuffer ptrBuff(4 * (xCnt * yCnt + 1));
			fd->GetRealData(ReadUInt32(&scaleTable[i * 4]) + 1036, 4 * (xCnt * yCnt + 1), ptrBuff);
			j = 0;
			currY = 0;
			while (currY < yCnt)
			{
				currX = 0;
				while (currX < xCnt)
				{
					thisOfst = ReadUInt32(&ptrBuff[j * 4]);
					nextOfst = ReadUInt32(&ptrBuff[(j + 1) * 4]);
					if ((nextOfst - thisOfst) <= 8192)
					{
						fd->GetRealData(thisOfst, nextOfst - thisOfst, srcBuff);
						if (srcBuff[0] == 0x78 && srcBuff[1] == 0xda)
						{
							inf.Decompress(imgBuff, decSize, srcBuff.SubArray(0, nextOfst - thisOfst));
							if (decSize == 4096)
							{
								imgX1 = (OSInt)currX << 6;
								imgY1 = (OSInt)currY << 6;
								imgX2 = imgX1 + 64;
								imgY2 = imgY1 + 64;
								if (imgX1 >= (OSInt)thisImgWidth || imgY1 >= (OSInt)thisImgHeight)
								{
								}
								else
								{
									if (imgY2 > (OSInt)thisImgHeight)
									{
										imgY2 = (OSInt)thisImgHeight;
									}
									if (imgX2 > (OSInt)thisImgWidth)
									{
										Data::ByteArray srcPtr = imgBuff + 4096;
										Data::ByteArray destPtr = outImg->GetDataArray() + (OSInt)thisImgWidth * imgY1 + imgX1;
										UOSInt w = thisImgWidth - (UOSInt)imgX1;
										while (imgY1 < imgY2)
										{
											srcPtr -= 64;
											destPtr.CopyFrom(srcPtr.WithSize(w));
											destPtr += thisImgWidth;
											imgY1++;
										}
									}
									else
									{
										Data::ByteArray srcPtr = imgBuff + 4096;
										Data::ByteArray destPtr = outImg->GetDataArray() + (OSInt)thisImgWidth * imgY1 + imgX1;
										while (imgY1 < imgY2)
										{
											srcPtr -= 64;
											destPtr.CopyFrom(srcPtr.WithSize(64));
											destPtr += thisImgWidth;
											imgY1++;
										}
									}
								}
							}
						}
					}
					j++;
					currX++;
				}
				currY++;
			}

			imgList->AddImage(outImg, 0);
		}
		i++;
	}
	return imgList;

/*	NEW_CLASS(outImg, Media::StaticImage(imgWidth, imgHeight, 0, bpp, 0, 0, Media::ColorProfile::YUVT_UNKNOWN, (bpp == 32)?Media::AT_ALPHA:Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	if (outImg)
	{
		UInt8 *pBits = (UInt8*)outImg->data;
		Int32 lineW;
		Int32 lineW2;
		Int32 currOfst;
		Int32 i;
		switch (bpp)
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
		case 8:
     		fd->GetRealData(endPos, 1024, outImg->pal);
			i = 256;
			while (i-- > 0)
			{
				outImg->pal[(i << 2) + 3] = 0xff;
			}
			lineW = imgWidth;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			currOfst = (lineW * imgHeight) + *(Int32*)&hdr[10];
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				fd->GetRealData(currOfst, imgWidth, pBits);
				pBits += imgWidth;
			}
			break;
		case 16:
			lineW = imgWidth << 1;
			if (lineW & 3)
			{
				lineW = lineW + 4 - (lineW & 3);
			}
			lineW2 = imgWidth << 1;
			currOfst = (lineW * imgHeight) + *(Int32*)&hdr[10];
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
			currOfst = (lineW * imgHeight) + *(Int32*)&hdr[10];
			while (imgHeight-- > 0)
			{
				currOfst -= lineW;
				fd->GetRealData(currOfst, lineW2, pBits);
				pBits += lineW2;
			}
			break;
		case 32:
			currOfst = *(Int32*)&hdr[10];
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
	}*/
}
