#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Media/ImageCopy_C.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/ICOParser.h"

Parser::FileParser::ICOParser::ICOParser()
{
}

Parser::FileParser::ICOParser::~ICOParser()
{
}

Int32 Parser::FileParser::ICOParser::GetName()
{
	return *(Int32*)"ICOP";
}

void Parser::FileParser::ICOParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.ico"), CSTR("Icon File"));
		selector->AddFilter(CSTR("*.cur"), CSTR("Cursor File"));
	}
}

IO::ParserType Parser::FileParser::ICOParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::ICOParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 icoImageHdr[16];
	UInt32 icoCnt;
	UOSInt i;
	Media::ImageList *imgList;
	NN<Media::StaticImage> currImg;
	UInt32 imgWidth;
	UInt32 imgHeight;
	UInt32 nextOfst;
	UInt32 thisSize;
	UInt16 bitCnt;
	UInt8 *pal;
	UnsafeArray<UInt8> dbits;
	OSInt dbpl;
	UInt16 fileType;

	if (ReadUInt16(&hdr[0]) != 0)
		return 0;
	fileType = ReadUInt16(&hdr[2]);
	if (fileType != 1 && fileType != 2)
	{
		return 0;
	}

	icoCnt = ReadUInt16(&hdr[4]);
	if (icoCnt > 16 || icoCnt < 1)
		return 0;

	nextOfst = 6 + (icoCnt << 4);
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
	i = 0;
	while (i < icoCnt)
	{
		fd->GetRealData(6 + (i << 4), 16, BYTEARR(icoImageHdr));

		imgWidth = icoImageHdr[0];
		imgHeight = icoImageHdr[1];
		if (imgWidth == 0)
			imgWidth = 256;
		if (imgHeight == 0)
			imgHeight = 256;
		thisSize = ReadUInt32(&icoImageHdr[8]);
		if (nextOfst != ReadUInt32(&icoImageHdr[12]) || thisSize > 1572904)
		{
			DEL_CLASS(imgList);
			return 0;
		}

		Data::ByteBuffer imgBuff(thisSize);
		fd->GetRealData(nextOfst, thisSize, imgBuff);
		if (ReadUInt32(&imgBuff[0]) != 40 || ReadUInt32(&imgBuff[4]) != imgWidth || ReadUInt32(&imgBuff[8]) != (imgHeight << 1))
		{
			DEL_CLASS(imgList);
			return 0;
		}
		pal = &imgBuff[40];
		bitCnt = ReadUInt16(&imgBuff[14]);

		switch (bitCnt)
		{
		case 1:
			{
				UInt8 *sptr = 8 + (UInt8*)pal;
				UInt8 *maskPtr;
				UnsafeArray<UInt8> currPtr;
				UOSInt i;
				UOSInt dataSize = (imgWidth + 7) >> 3;
				UOSInt dataAdd;
				dbpl = (OSInt)dataSize * 2;
				if ((dataAdd = (dataSize & 3)) != 0)
				{
					dataAdd = 4 - dataAdd;
				}
				maskPtr = sptr + (dataSize + dataAdd) * imgHeight;

				if (thisSize != (dataSize + dataAdd) * imgHeight * 2 + 40 + 8)
				{
					DEL_CLASS(imgList);
					return 0;
				}

				NEW_CLASSNN(currImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, 2, Media::PF_PAL_1_A1, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dbits = currImg->data;
				MemCopyNO(currImg->pal, pal, 8);

				dbits += dbpl * (OSInt)imgHeight;
				while (imgHeight-- > 0)
				{
					dbits -= dbpl;

					currPtr = dbits;
					MemCopyNO(currPtr.Ptr(), sptr, dataSize);
					currPtr += dataSize;
					sptr += dataSize + dataAdd;

					i = dataSize;
					while (i-- > 0)
					{
						*currPtr++ = (UInt8)(~*maskPtr++);
					}

					maskPtr += dataAdd;
				}
			}
			break;
		case 4:
			{
				UInt8 *sptr = 64 + (UInt8*)pal;
				UInt8 *maskPtr;;
				UnsafeArray<UInt8> currPtr;
				UOSInt i;
				UOSInt maskByteSize = (imgWidth + 7) >> 3;
				UOSInt imgByteSize = (imgWidth + 1) >> 1;
				UOSInt maskByteAdd;
				UOSInt imgByteAdd;
				dbpl = (OSInt)(maskByteSize + imgByteSize);

				if ((maskByteAdd = (maskByteSize & 3)) != 0)
				{
					maskByteAdd = 4 - maskByteAdd;
				}
				if ((imgByteAdd = (imgByteSize & 3)) != 0)
				{
					imgByteAdd = 4 - imgByteAdd;
				}
				maskPtr = sptr + (imgByteSize + imgByteAdd) * imgHeight;

				if (thisSize != (imgByteSize + imgByteAdd + maskByteSize + maskByteAdd) * imgHeight + 40 + 64)
				{
					DEL_CLASS(imgList);
					return 0;
				}

				NEW_CLASSNN(currImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, 5, Media::PF_PAL_4_A1, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dbits = currImg->data;
				MemCopyNO(currImg->pal, pal, 64);

				dbits += dbpl * (OSInt)imgHeight;
				while (imgHeight-- > 0)
				{
					dbits -= dbpl;

					currPtr = dbits;
					MemCopyNO(currPtr.Ptr(), sptr, imgByteSize);
					currPtr += imgByteSize;
					sptr += imgByteSize + imgByteAdd;

					i = maskByteSize;
					while (i-- > 0)
					{
						*currPtr++ = (UInt8)~*maskPtr++;
					}

					maskPtr += maskByteAdd;
				}
			}
			break;
		case 8:
			{
				UInt8 *sptr = 1024 + (UInt8*)pal;
				UInt8 *maskPtr;
				UnsafeArray<UInt8> currPtr;
				UOSInt i;
				UOSInt maskByteSize = (imgWidth + 7) >> 3;
				UOSInt maskByteAdd;
				UOSInt imgByteAdd;
				dbpl = (OSInt)(maskByteSize + imgWidth);

				if ((maskByteAdd = (maskByteSize & 3)) != 0)
				{
					maskByteAdd = 4 - maskByteAdd;
				}
				if ((imgByteAdd = (imgWidth & 3)) != 0)
				{
					imgByteAdd = 4 - imgByteAdd;
				}
				maskPtr = sptr + (imgWidth + imgByteAdd) * imgHeight;

				if (thisSize != (imgWidth + imgByteAdd + maskByteSize + maskByteAdd) * imgHeight + 40 + 1024)
				{
					DEL_CLASS(imgList);
					return 0;
				}

				NEW_CLASSNN(currImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, 9, Media::PF_PAL_8_A1, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dbits = currImg->data;
				MemCopyNO(currImg->pal, pal, 1024);

				dbits += dbpl * (OSInt)imgHeight;
				while (imgHeight-- > 0)
				{
					dbits -= dbpl;

					currPtr = dbits;
					MemCopyNO(currPtr.Ptr(), sptr, imgWidth);
					currPtr += imgWidth;
					sptr += imgWidth + imgByteAdd;

					i = maskByteSize;
					while (i-- > 0)
					{
						*currPtr++ = (UInt8)~*maskPtr++;
					}

					maskPtr += maskByteAdd;
				}
			}
			break;
		case 24:
			{
				UInt8 *sptr = (UInt8*)pal;
				UInt8 *maskPtr;
				UnsafeArray<UInt8> currPtr;
				UOSInt i;
				OSInt maskByteSize = (OSInt)(imgWidth + 7) >> 3;
				OSInt maskByteAdd;
				OSInt imgByteAdd;
				dbpl = (OSInt)imgWidth * 4;

				if ((maskByteAdd = (maskByteSize & 3)) != 0)
				{
					maskByteAdd = 4 - maskByteAdd;
				}
				if ((imgByteAdd = ((imgWidth * 3) & 3)) != 0)
				{
					imgByteAdd = 4 - imgByteAdd;
				}
				maskPtr = sptr + ((OSInt)imgWidth * 3 + imgByteAdd) * (OSInt)imgHeight;

				if ((OSInt)thisSize != ((OSInt)imgWidth * 3 + (OSInt)imgByteAdd + maskByteSize + maskByteAdd) * (OSInt)imgHeight + 40)
				{
					DEL_CLASS(imgList);
					return 0;
				}

				NEW_CLASSNN(currImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, 32, Media::PF_B8G8R8A1, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dbits = currImg->data;

				dbits += dbpl * (OSInt)imgHeight;
				while (imgHeight-- > 0)
				{
					dbits -= dbpl;

					currPtr = dbits;
					i = imgWidth;
					while (i-- > 0)
					{
						currPtr[0] = sptr[0];
						currPtr[1] = sptr[1];
						currPtr[2] = sptr[2];
						currPtr[3] = 0;
						currPtr += 4;
						sptr += 3;
					}
					sptr += imgByteAdd;

					currPtr = dbits;
					i = imgWidth >> 3;
					while (i-- > 0)
					{
						currPtr[3] = (UInt8)((maskPtr[0] >> 7) - 1);
						currPtr[7] = (UInt8)(((maskPtr[0] >> 6) & 1) - 1);
						currPtr[11] = (UInt8)(((maskPtr[0] >> 5) & 1) - 1);
						currPtr[15] = (UInt8)(((maskPtr[0] >> 4) & 1) - 1);
						currPtr[19] = (UInt8)(((maskPtr[0] >> 3) & 1) - 1);
						currPtr[23] = (UInt8)(((maskPtr[0] >> 2) & 1) - 1);
						currPtr[27] = (UInt8)(((maskPtr[0] >> 1) & 1) - 1);
						currPtr[31] = (UInt8)(((maskPtr[0] >> 0) & 1) - 1);
						currPtr += 32;
						maskPtr++;
					}
					if (imgWidth & 7)
					{
						UInt8 v = maskPtr[0];
						maskPtr++;
						i = imgWidth & 7;
						while (i-- > 0)
						{
							currPtr[3] = (UInt8)((v >> 7) - 1);
							v = (UInt8)(v << 1);
							currPtr += 4;
						}
					}
					maskPtr += maskByteAdd;
				}
			}
			break;
		case 32:
			{
				UOSInt maskByteSize = (imgWidth + 7) >> 3;
				UOSInt maskByteAdd;
				if ((maskByteAdd = (maskByteSize & 3)) != 0)
				{
					maskByteAdd = 4 - maskByteAdd;
				}
				if (thisSize != (UInt32)((imgWidth * 4) * imgHeight + 40) && thisSize != (imgWidth * 4 + maskByteSize + maskByteAdd) * imgHeight + 40)
				{
					DEL_CLASS(imgList);
					return 0;
				}

				NEW_CLASSNN(currImg, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, 32, Media::PF_B8G8R8A8, 0, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_ALPHA, Media::YCOFST_C_CENTER_LEFT));
				dbits = currImg->data;
				ImageCopy_ImgCopy(imgWidth * 4 * (imgHeight - 1) + (UInt8*)pal, dbits.Ptr(), imgWidth * 4, imgHeight, -(OSInt)imgWidth * 4, (OSInt)imgWidth * 4);
			}
			break;
		default:
			DEL_CLASS(imgList);
			return 0;
		}
		if (fileType == 2)
		{
			currImg->SetHotSpot(ReadUInt16(&icoImageHdr[4]), ReadUInt16(&icoImageHdr[6]));
		}

		imgList->AddImage(currImg, 0);
		i++;
		nextOfst += thisSize;
	}

	return imgList;
}
