#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/PackBits.h"
#include "Data/Compress/LZWDecompressor.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/VectorImage.h"
#include "Media/FrameInfo.h"
#include "Media/ICCProfile.h"
#include "Media/IImgResizer.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Parser::FileParser::TIFFParser::TIFFParser()
{
	this->parsers = 0;
}

Parser::FileParser::TIFFParser::~TIFFParser()
{
}

Int32 Parser::FileParser::TIFFParser::GetName()
{
	return *(Int32*)"TIFP";
}

void Parser::FileParser::TIFFParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::TIFFParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter((const UTF8Char*)"*.tif", (const UTF8Char*)"TIFF File");
		selector->AddFilter((const UTF8Char*)"*.tiff", (const UTF8Char*)"TIFF File");
	}
}

IO::ParserType Parser::FileParser::TIFFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::TIFFParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[8];
	Media::EXIFData::RInt32Func readInt32;
	Media::EXIFData::RInt16Func readInt16;
	Media::EXIFData::RFloatFunc readFloat16;
	Media::EXIFData::RFloatFunc readFloat24;
	Media::EXIFData::RFloatFunc readFloat32;
//	Media::EXIFData::WInt32Func writeInt32;
//	Media::EXIFData::WInt16Func writeInt16;
	Bool isBE;

	UInt32 nextOfst;
	UOSInt i;

	if (fd->GetRealData(0, 8, hdr) != 8)
		return 0;
	if (*(Int16*)&hdr[0] == *(Int16*)"MM")
	{
		readInt16 = Media::EXIFData::TReadMInt16;
		readInt32 = Media::EXIFData::TReadMInt32;
		readFloat16 = Media::EXIFData::TReadMFloat16;
		readFloat24 = Media::EXIFData::TReadMFloat24;
		readFloat32 = Media::EXIFData::TReadMFloat32;
//		writeInt16 = Media::EXIFData::TWriteMInt16;
//		writeInt32 = Media::EXIFData::TWriteMInt32;
		isBE = true;
	}
	else if (*(Int16*)&hdr[0] == *(Int16*)"II")
	{
		readInt16 = Media::EXIFData::TReadInt16;
		readInt32 = Media::EXIFData::TReadInt32;
		readFloat16 = Media::EXIFData::TReadFloat16;
		readFloat24 = Media::EXIFData::TReadFloat24;
		readFloat32 = Media::EXIFData::TReadFloat32;
//		writeInt16 = Media::EXIFData::TWriteInt16;
//		writeInt32 = Media::EXIFData::TWriteInt32;
		isBE = false;
	}
	else
	{
		return 0;
	}
	if (readInt16(&hdr[2]) != 42)
		return 0;
	
	nextOfst = (UInt32)readInt32(&hdr[4]);
	
	Media::ImageList *imgList;
	Media::StaticImage *img;
	Media::EXIFData *exif;

	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
	while (nextOfst)
	{
		img = 0;
		exif = Media::EXIFData::ParseIFD(fd, nextOfst, readInt32, readInt16, &nextOfst, 0);
		if (exif == 0)
		{
			DEL_CLASS(imgList);
			return 0;
		}

		Bool valid = true;
		Bool processed = false;
		UOSInt j;
		UOSInt k;
		UOSInt nChannels;
		UInt32 compression = 0;
		UInt32 imgWidth = 0;
		UInt32 imgHeight = 0;
		UInt32 bpp = 0;
		UInt32 photometricInterpretation = 0;
		UInt32 planarConfiguration = 1;
		UInt32 sampleFormat = 1;
		UInt32 nStrip = 0;
		UInt32 stripOfst = 0;
		UInt32 stripLeng = 0;
		UInt32 *stripOfsts = 0;
		UInt32 *stripLengs = 0;
		UInt32 storeBPP;
		UInt32 predictor;
		UInt32 rowsPerStrip;
		if ((imgWidth = GetUInt(exif, 0x100)) == 0)
			valid = false;
		if ((imgHeight = GetUInt(exif, 0x101)) == 0)
			valid = false;
		if ((bpp = GetUIntSum(exif, 0x102, &nChannels)) == 0)
			valid = false;
		sampleFormat = GetUInt0(exif, 0x153);
		if (sampleFormat == 0)
			sampleFormat = 1;
		predictor = GetUInt(exif, 0x13d);
		if (predictor == 0)
			predictor = 1;
		compression = GetUInt(exif, 0x103);
		photometricInterpretation = GetUInt(exif, 0x106);
		planarConfiguration = GetUInt(exif, 284);
		rowsPerStrip = GetUInt(exif, 278);
		storeBPP = bpp;
		Media::EXIFData::EXIFItem *item = exif->GetExifItem(0x111);
		Media::PixelFormat pf = Media::PixelFormatGetDef(0, bpp);
		if (item == 0)
		{
			valid = false;
		}
		else if (item->size == 1)
		{
			if (item->type == Media::EXIFData::ET_UINT16)
			{
				stripOfst = (UInt16)item->value;
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripOfst = (UInt32)item->value;
			}
			else
			{
				valid = false;
			}
		}
		else
		{
			if (item->type == Media::EXIFData::ET_UINT16)
			{
				UInt16 *val = exif->GetExifUInt16(0x111);
				stripOfsts = MemAlloc(UInt32, item->size);
				j = item->size;
				while (j-- > 0)
				{
					stripOfsts[j] = val[j];
				}
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripOfsts = MemAlloc(UInt32, item->size);
				MemCopyNO(stripOfsts, exif->GetExifUInt32(0x111), item->size * sizeof(UInt32));
			}
			else
			{
				valid = false;
			}
		}
		item = exif->GetExifItem(0x117);
		if (item == 0)
		{
			valid = false;
		}
		else if (item->size == 1)
		{
			nStrip = 1;
			if (item->type == Media::EXIFData::ET_UINT16)
			{
				stripLeng = (UInt16)item->value;
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripLeng = (UInt32)item->value;
			}
			else
			{
				valid = false;
			}
		}
		else
		{
			nStrip = item->size;
			if (item->type == Media::EXIFData::ET_UINT16)
			{
				UInt16 *val = exif->GetExifUInt16(0x111);
				stripLengs = MemAlloc(UInt32, item->size);
				j = item->size;
				while (j-- > 0)
				{
					stripLengs[j] = val[j];
				}
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripLengs = MemAlloc(UInt32, item->size);
				MemCopyNO(stripLengs, exif->GetExifUInt32(0x117), item->size * sizeof(UInt32));
			}
			else
			{
				valid = false;
			}
		}

		if (compression == 1) // no compression
		{
		}
		else if (compression == 32773) // PackBits
		{
		}
		else if (compression == 5) //LZW
		{
		}
		else if (compression == 7) //JPEG
		{
			if (nStrip == 1)
			{
				Media::ImageList *innerImgList = 0;
				IO::IStreamData *jpgFd = fd->GetPartialData(stripOfst, stripLeng);
				if (this->parsers)
				{
					innerImgList = (Media::ImageList*)this->parsers->ParseFileType(jpgFd, IO::ParserType::ImageList); 
				}
				DEL_CLASS(jpgFd);
				if (innerImgList)
				{
					Media::Image *innerImg;
					Media::StaticImage *innerSImg;
					UInt32 imgDelay;
					i = 0;
					j = innerImgList->GetCount();
					while (i < j)
					{
						innerImg = innerImgList->GetImage(i, &imgDelay);
						innerSImg = innerImg->CreateStaticImage();
						if (exif)
						{
							innerSImg->SetEXIFData(exif);
							exif = 0;
						}
						i++;
					}
					DEL_CLASS(innerImgList);
					if (exif)
					{
						DEL_CLASS(exif);
						exif = 0;
					}
					if (stripOfsts)
					{
						MemFree(stripOfsts);
					}
					if (stripLengs)
					{
						MemFree(stripLengs);
					}
					processed = true;
				}
				else
				{
					valid = false;
				}
			}
			else
			{
				valid = false;
			}
		}
		else if (compression == 8) // ZIP Compression
		{
		}
		else
		{
			valid = false;
		}

		if (sampleFormat == 3)
		{
			if (nChannels == 1 && bpp <= 32)
			{
				pf = Media::PF_LE_FW32;
				storeBPP = 32;
			}
			else if (nChannels == 2 && bpp <= 64)
			{
				pf = Media::PF_LE_FW32A32;
				storeBPP = 64;
			}
			else if (nChannels == 3 && bpp <= 96)
			{
				pf = Media::PF_LE_FB32G32R32;
				storeBPP = 96;
			}
			else if (nChannels == 4 && bpp <= 128)
			{
				pf = Media::PF_LE_FB32G32R32A32;
				storeBPP = 128;
			}
		}
		else
		{
			if (photometricInterpretation == 1 || photometricInterpretation == 2)
			{
				if (nChannels == 1)
				{
					if (bpp == 1)
					{
						pf = Media::PF_PAL_W1;
					}
					else if (bpp == 2)
					{
						pf = Media::PF_PAL_W2;
					}
					else if (bpp == 4)
					{
						pf = Media::PF_PAL_W4;
					}
					else if (bpp == 8)
					{
						pf = Media::PF_PAL_W8;
					}
					else if (bpp == 16)
					{
						pf = Media::PF_LE_W16;
					}
				}
				else if (nChannels == 2)
				{
					if (bpp == 16)
					{
						pf = Media::PF_W8A8;
					}
					else if (bpp == 32)
					{
						pf = Media::PF_LE_W16A16;
					}
				}
			}
		}

		if (!valid)
		{
			DEL_CLASS(exif);
			if (stripOfsts)
			{
				MemFree(stripOfsts);
			}
			if (stripLengs)
			{
				MemFree(stripLengs);
			}
			if (imgList->GetCount() > 0)
			{
				return imgList;
			}
			else
			{
				DEL_CLASS(imgList);
				return 0;
			}
		}
		else if (processed)
		{
		}
		else
		{
			Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
			item = exif->GetExifItem(34675);
			if (item)
			{
				Media::ICCProfile *icc;
				icc = Media::ICCProfile::Parse((UInt8*)item->dataBuff, item->size);
				if (icc)
				{
					icc->GetRedTransferParam(color.GetRTranParam());
					icc->GetGreenTransferParam(color.GetGTranParam());
					icc->GetBlueTransferParam(color.GetBTranParam());
					icc->GetColorPrimaries(color.GetPrimaries());
					color.SetRAWICC((UInt8*)item->dataBuff);
					DEL_CLASS(icc);
				}
			}

			NEW_CLASS(img, Media::StaticImage(imgWidth, imgHeight, 0, storeBPP, pf, 0, &color, Media::ColorProfile::YUVT_UNKNOWN, (bpp == 32 || bpp == 64)?Media::AT_ALPHA:Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			UInt8 *imgData;
			UInt8 *planarBuff = 0;
			if (sampleFormat == 3)
			{
				planarBuff = MemAlloc(UInt8, imgWidth * imgHeight * bpp >> 3);
				imgData = planarBuff;
			}
			else if (planarConfiguration == 2 && nChannels > 1)
			{
				planarBuff = MemAlloc(UInt8, imgWidth * imgHeight * bpp >> 3);
				imgData = planarBuff;
			}
			else
			{
				imgData = (UInt8*)img->data;
			}
			if (compression == 32773)
			{
				UInt8 *compImgData;
				UOSInt maxLeng;
				UOSInt lengLeft;
				UOSInt destSize;
				if (nStrip == 1)
				{
					compImgData = MemAlloc(UInt8, stripLeng);
					lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
					Data::Compress::PackBits::Decompress(imgData, &destSize, compImgData, lengLeft);
					MemFree(compImgData);
				}
				else
				{
					maxLeng = stripLengs[i = nStrip - 1];
					while (i-- > 0)
					{
						if (stripLengs[i] > maxLeng)
							maxLeng = stripLengs[i];
					}
					compImgData = MemAlloc(UInt8, maxLeng);

//					OSInt dbpl = imgWidth * bpp >> 3;
//					OSInt decBpl = (((imgWidth * bpp) & ~7) + 7)/ 8;
					i = 0;
					while (i < nStrip)
					{
						//fd->GetRealData(stripOfsts[i], stripLengs[i], imgData);
						//imgData += stripLengs[i];

						lengLeft = fd->GetRealData(stripOfsts[i], stripLengs[i], compImgData);
						Data::Compress::PackBits::Decompress(imgData, &destSize, compImgData, lengLeft);
						imgData += destSize;

						i++;
					}
					MemFree(compImgData);
				}
			}
			else if (compression == 5 || compression == 8)
			{
				UInt8 *compImgData;
				UOSInt maxLeng;
				UOSInt lengLeft;
				UOSInt destSize;
				if (compression == 5)
				{
					Data::Compress::LZWDecompressor lzw;
					if (nStrip == 1)
					{
						compImgData = MemAlloc(UInt8, stripLeng);
						lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
						lzw.Decompress(imgData, &destSize, compImgData, lengLeft);
						MemFree(compImgData);
					}
					else
					{
						maxLeng = stripLengs[i = nStrip - 1];
						while (i-- > 0)
						{
							if (stripLengs[i] > maxLeng)
								maxLeng = stripLengs[i];
						}
						compImgData = MemAlloc(UInt8, maxLeng);

						UOSInt dbpl = imgWidth * bpp >> 3;
//						OSInt decBpl = (((imgWidth * bpp) & ~7) + 7)/ 8;
						UOSInt decSize = dbpl * rowsPerStrip;
						i = 0;
						while (i < nStrip)
						{
							lengLeft = fd->GetRealData(stripOfsts[i], stripLengs[i], compImgData);
							destSize = 0;
							if (i == 4748)
							{
								i = 4748;
							}
							lzw.Decompress(imgData, &destSize, compImgData, lengLeft);
							if (destSize != decSize)
							{
								destSize = decSize;
							}
							imgData += destSize;

							i++;
						}
						MemFree(compImgData);
					}
				}
				else if (compression == 8)
				{
					Data::Compress::Inflate inflate;
					if (nStrip == 1)
					{
						compImgData = MemAlloc(UInt8, stripLeng);
						lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
						inflate.Decompress(imgData, &destSize, compImgData, lengLeft);
						MemFree(compImgData);
					}
					else
					{
						maxLeng = stripLengs[i = nStrip - 1];
						while (i-- > 0)
						{
							if (stripLengs[i] > maxLeng)
								maxLeng = stripLengs[i];
						}
						compImgData = MemAlloc(UInt8, maxLeng);

//						OSInt dbpl = imgWidth * bpp >> 3;
//						OSInt decBpl = (((imgWidth * bpp) & ~7) + 7)/ 8;
						i = 0;
						while (i < nStrip)
						{
							//fd->GetRealData(stripOfsts[i], stripLengs[i], imgData);
							//imgData += stripLengs[i];

							lengLeft = fd->GetRealData(stripOfsts[i], stripLengs[i], compImgData);
							inflate.Decompress(imgData, &destSize, compImgData, lengLeft);
							imgData += destSize;

							i++;
						}
						MemFree(compImgData);
					}
				}
			}
			else
			{
				if (nStrip == 1)
				{
					fd->GetRealData(stripOfst, stripLeng, imgData);
				}
				else
				{
					i = 0;
					while (i < nStrip)
					{
						fd->GetRealData(stripOfsts[i], stripLengs[i], imgData);
						imgData += stripLengs[i];
						i++;
					}
				}
			}

			if (sampleFormat == 3 && predictor == 3)
			{
				UInt8 *tmpBuff;
				UInt8 *tmpPtr;
				UInt8 *tmpPtr2;
				UInt8 lastPx[4];
				UOSInt bytePerChannels = (bpp >> 3) / nChannels;
				UOSInt l;
				if (planarConfiguration == 2)
				{
					tmpBuff = MemAlloc(UInt8, bytePerChannels * imgWidth);
					imgData = planarBuff;
					if (isBE)
					{
						i = nChannels;
						while (i-- > 0)
						{
							j = imgHeight;
							while (j-- > 0)
							{
								tmpPtr = imgData;
								k = bytePerChannels;
								while (k-- > 0)
								{
									tmpPtr2 = tmpBuff + bytePerChannels - k - 1;
									lastPx[0] = 0;
									l = imgWidth;
									while (l-- > 0)
									{
										lastPx[0] = (UInt8)(lastPx[0] + *tmpPtr++);
										tmpPtr2[0] = lastPx[0];
										tmpPtr2 += bytePerChannels;
									}
								}
								MemCopyNO(imgData, tmpBuff, bytePerChannels * imgWidth);
								imgData += bytePerChannels * imgWidth;
							}
						}
					}
					else
					{
						i = nChannels;
						while (i-- > 0)
						{
							j = imgHeight;
							while (j-- > 0)
							{
								tmpPtr = imgData;
								k = bytePerChannels;
								while (k-- > 0)
								{
									tmpPtr2 = tmpBuff;
									lastPx[0] = 0;
									l = imgWidth;
									while (l-- > 0)
									{
										lastPx[0] = (UInt8)(lastPx[0] + *tmpPtr++);
										tmpPtr2[k] = lastPx[0];
										tmpPtr2 += bytePerChannels;
									}
								}
								MemCopyNO(imgData, tmpBuff, bytePerChannels * imgWidth);
								imgData += bytePerChannels * imgWidth;
							}
						}
					}
					MemFree(tmpBuff);
				}
				else
				{
					tmpBuff = MemAlloc(UInt8, bytePerChannels * imgWidth * nChannels);
					/////////////////////////////////////////////////////
					if (isBE)
					{
						imgData = planarBuff;
						i = imgHeight;
						while (i-- > 0)
						{
							tmpPtr = imgData;
							j = bytePerChannels;
							while (j-- > 0)
							{
								tmpPtr2 = tmpBuff + bytePerChannels - j - 1;
								k = nChannels;
								while (k-- > 0)
								{
									lastPx[k] = 0;
								}
								k = imgWidth;
								while (k-- > 0)
								{
									l = nChannels;
									while (l-- > 0)
									{
										lastPx[l] = (UInt8)(lastPx[l] + *tmpPtr++);
										tmpPtr2[0] = lastPx[l];
										tmpPtr2 += bytePerChannels;
									}
								}
							}
							MemCopyNO(imgData, tmpBuff, bytePerChannels * imgWidth * nChannels);
							imgData = tmpPtr;
						}
					}
					else
					{
						imgData = planarBuff;
						i = imgHeight;
						while (i-- > 0)
						{
							tmpPtr = imgData;
							j = bytePerChannels;
							while (j-- > 0)
							{
								tmpPtr2 = tmpBuff + j;
								k = nChannels;
								while (k-- > 0)
								{
									lastPx[k] = 0;
								}
								k = imgWidth;
								while (k-- > 0)
								{
									l = nChannels;
									while (l-- > 0)
									{
										lastPx[l] = (UInt8)(lastPx[l] + *tmpPtr++);
										tmpPtr2[0] = lastPx[l];
										tmpPtr2 += bytePerChannels;
									}
								}
							}
							MemCopyNO(imgData, tmpBuff, bytePerChannels * imgWidth * nChannels);
							imgData = tmpPtr;
						}
					}
					MemFree(tmpBuff);
				}
			}

			if (sampleFormat == 3)
			{
				if (nChannels == 1)
				{
					if (bpp == 16)
					{
						UInt8 *srcPtr = planarBuff;
						imgData = (UInt8*)img->data;
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], readFloat16(&srcPtr[0]));
							srcPtr += 2;
							imgData += 4;
						}
					}
					else if (bpp == 24)
					{
						UInt8 *srcPtr = planarBuff;
						imgData = (UInt8*)img->data;
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], readFloat24(&srcPtr[0]));
							srcPtr += 3;
							imgData += 4;
						}
					}
					else if (bpp == 32)
					{
						UInt8 *srcPtr = planarBuff;
						imgData = (UInt8*)img->data;
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], readFloat32(&srcPtr[0]));
							srcPtr += 4;
							imgData += 4;
						}
					}
				}
				else if (nChannels == 2)
				{
					if (bpp == 32)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *wPtr = planarBuff;
							UInt8 *aPtr = wPtr + imgWidth * imgHeight * 2;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(wPtr));
								WriteFloat(&imgData[4], readFloat16(aPtr));
								wPtr += 2;
								aPtr += 2;
								imgData += 8;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(&srcPtr[0]));
								WriteFloat(&imgData[4], readFloat16(&srcPtr[2]));
								srcPtr += 4;
								imgData += 8;
							}
						}
					}
					else if (bpp == 48)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *wPtr = planarBuff;
							UInt8 *aPtr = wPtr + imgWidth * imgHeight * 3;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(wPtr));
								WriteFloat(&imgData[4], readFloat24(aPtr));
								wPtr += 3;
								aPtr += 3;
								imgData += 8;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(&srcPtr[0]));
								WriteFloat(&imgData[4], readFloat24(&srcPtr[3]));
								srcPtr += 6;
								imgData += 8;
							}
						}
					}
					else if (bpp == 64)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *wPtr = planarBuff;
							UInt8 *aPtr = wPtr + imgWidth * imgHeight * 4;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(wPtr));
								WriteFloat(&imgData[4], readFloat32(aPtr));
								wPtr += 4;
								aPtr += 4;
								imgData += 8;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(&srcPtr[0]));
								WriteFloat(&imgData[4], readFloat32(&srcPtr[4]));
								srcPtr += 8;
								imgData += 8;
							}
						}
					}
				}
				else if (nChannels == 3)
				{
					if (bpp == 48)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 2;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 2;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(bPtr));
								WriteFloat(&imgData[4], readFloat16(gPtr));
								WriteFloat(&imgData[8], readFloat16(rPtr));
								rPtr += 2;
								gPtr += 2;
								bPtr += 2;
								imgData += 12;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(&srcPtr[4]));
								WriteFloat(&imgData[4], readFloat16(&srcPtr[2]));
								WriteFloat(&imgData[8], readFloat16(&srcPtr[0]));
								srcPtr += 6;
								imgData += 12;
							}
						}
					}
					else if (bpp == 72)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 3;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 3;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(bPtr));
								WriteFloat(&imgData[4], readFloat24(gPtr));
								WriteFloat(&imgData[8], readFloat24(rPtr));
								rPtr += 3;
								gPtr += 3;
								bPtr += 3;
								imgData += 12;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(&srcPtr[6]));
								WriteFloat(&imgData[4], readFloat24(&srcPtr[3]));
								WriteFloat(&imgData[8], readFloat24(&srcPtr[0]));
								srcPtr += 9;
								imgData += 12;
							}
						}
					}
					else if (bpp == 96)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 4;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 4;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(bPtr));
								WriteFloat(&imgData[4], readFloat32(gPtr));
								WriteFloat(&imgData[8], readFloat32(rPtr));
								rPtr += 4;
								gPtr += 4;
								bPtr += 4;
								imgData += 12;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(&srcPtr[8]));
								WriteFloat(&imgData[4], readFloat32(&srcPtr[4]));
								WriteFloat(&imgData[8], readFloat32(&srcPtr[0]));
								srcPtr += 12;
								imgData += 12;
							}
						}
					}
				}
				else if (nChannels == 4)
				{
					if (bpp == 64)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 2;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 2;
							UInt8 *aPtr = bPtr + imgWidth * imgHeight * 2;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(bPtr));
								WriteFloat(&imgData[4], readFloat16(gPtr));
								WriteFloat(&imgData[8], readFloat16(rPtr));
								WriteFloat(&imgData[12], readFloat16(aPtr));
								rPtr += 2;
								gPtr += 2;
								bPtr += 2;
								aPtr += 2;
								imgData += 16;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat16(&srcPtr[4]));
								WriteFloat(&imgData[4], readFloat16(&srcPtr[2]));
								WriteFloat(&imgData[8], readFloat16(&srcPtr[0]));
								WriteFloat(&imgData[12], readFloat16(&srcPtr[6]));
								srcPtr += 8;
								imgData += 16;
							}
						}
					}
					else if (bpp == 96)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 3;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 3;
							UInt8 *aPtr = bPtr + imgWidth * imgHeight * 3;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(bPtr));
								WriteFloat(&imgData[4], readFloat24(gPtr));
								WriteFloat(&imgData[8], readFloat24(rPtr));
								WriteFloat(&imgData[12], readFloat24(aPtr));
								rPtr += 3;
								gPtr += 3;
								bPtr += 3;
								aPtr += 3;
								imgData += 16;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat24(&srcPtr[6]));
								WriteFloat(&imgData[4], readFloat24(&srcPtr[3]));
								WriteFloat(&imgData[8], readFloat24(&srcPtr[0]));
								WriteFloat(&imgData[12], readFloat24(&srcPtr[9]));
								srcPtr += 12;
								imgData += 16;
							}
						}
					}
					else if (bpp == 128)
					{
						if (planarConfiguration == 2)
						{
							UInt8 *rPtr = planarBuff;
							UInt8 *gPtr = rPtr + imgWidth * imgHeight * 4;
							UInt8 *bPtr = gPtr + imgWidth * imgHeight * 4;
							UInt8 *aPtr = bPtr + imgWidth * imgHeight * 4;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(bPtr));
								WriteFloat(&imgData[4], readFloat32(gPtr));
								WriteFloat(&imgData[8], readFloat32(rPtr));
								WriteFloat(&imgData[12], readFloat32(aPtr));
								rPtr += 4;
								gPtr += 4;
								bPtr += 4;
								aPtr += 4;
								imgData += 16;
							}
						}
						else
						{
							UInt8 *srcPtr = planarBuff;
							imgData = (UInt8*)img->data;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], readFloat32(&srcPtr[8]));
								WriteFloat(&imgData[4], readFloat32(&srcPtr[4]));
								WriteFloat(&imgData[8], readFloat32(&srcPtr[0]));
								WriteFloat(&imgData[12], readFloat32(&srcPtr[12]));
								srcPtr += 16;
								imgData += 16;
							}
						}
					}
				}
			}
			else if (bpp <= 8)
			{
				UOSInt j;
				if (photometricInterpretation == 3)
				{
					UInt16 *pal = exif->GetExifUInt16(0x140);
					j = (UOSInt)1 << bpp;
					i = 0;
					while (i < j)
					{
						img->pal[i << 2] = (UInt8)(pal[(j << 1) + i] >> 8);
						img->pal[(i << 2) + 1] = (UInt8)(pal[j + i] >> 8);
						img->pal[(i << 2) + 2] = (UInt8)(pal[i] >> 8);
						img->pal[(i << 2) + 3] = 0xff;
						i++;
					}
				}
				else if (photometricInterpretation == 0) //whiteIsZero
				{
					UInt8 v;
					i = (UOSInt)1 << bpp;
					j = i - 1;
					while (i-- > 0)
					{
						v = (UInt8)(i * 255 / j);
						img->pal[i << 2] = v;
						img->pal[(i << 2) + 1] = v;
						img->pal[(i << 2) + 2] = v;
						img->pal[(i << 2) + 3] = 0xff;
					}

					imgData = (UInt8*)img->data;
					i = ((imgWidth * bpp + 7) >> 3) * imgHeight;
					while (i-- > 0)
					{
						imgData[0] = (UInt8)(255 - imgData[0]);
						imgData += 1;
					}
				}
				else if (photometricInterpretation == 1) //blackIsZero
				{
					UInt8 v;
					i = (UOSInt)1 << bpp;
					j = i - 1;
					while (i-- > 0)
					{
						v = (UInt8)(i * 255 / j);
						img->pal[i << 2] = v;
						img->pal[(i << 2) + 1] = v;
						img->pal[(i << 2) + 2] = v;
						img->pal[(i << 2) + 3] = 0xff;
					}
				}
			}
			else if (nChannels == 1 && bpp == 16)
			{
				imgData = (UInt8*)img->data;
				i = imgWidth * imgHeight;
				while (i-- > 0)
				{
					WriteInt16(&imgData[0], readInt16(&imgData[0]));
					imgData += 2;
				}
			}
			else if (nChannels == 2 && bpp == 16)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *wPtr = planarBuff;
					UInt8 *aPtr = wPtr + imgWidth * imgHeight;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						imgData[0] = *wPtr++;
						imgData[1] = *aPtr++;
						imgData += 2;
					}
				}
				else
				{
				}
			}
			else if (nChannels == 2 && bpp == 32)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *wPtr = planarBuff;
					UInt8 *aPtr = wPtr + imgWidth * imgHeight * 2;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], readInt16(wPtr));
						WriteInt16(&imgData[2], readInt16(aPtr));
						wPtr += 2;
						aPtr += 2;
						imgData += 4;
					}
				}
				else
				{
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], readInt16(&imgData[0]));
						WriteInt16(&imgData[2], readInt16(&imgData[2]));
						imgData += 4;
					}
				}
			}
			else if (nChannels == 3 && bpp == 24)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *rPtr = planarBuff;
					UInt8 *gPtr = rPtr + imgWidth * imgHeight;
					UInt8 *bPtr = gPtr + imgWidth * imgHeight;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						imgData[0] = *bPtr++;
						imgData[1] = *gPtr++;
						imgData[2] = *rPtr++;
						imgData += 3;
					}
				}
				else
				{
					UInt8 tmpByte;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						tmpByte = imgData[0];
						imgData[0] = imgData[2];
						imgData[2] = tmpByte;
						imgData += 3;
					}
				}
			}
			else if (nChannels == 4 && bpp == 32)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *rPtr = planarBuff;
					UInt8 *gPtr = rPtr + imgWidth * imgHeight;
					UInt8 *bPtr = gPtr + imgWidth * imgHeight;
					UInt8 *aPtr = bPtr + imgWidth * imgHeight;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						imgData[0] = *bPtr++;
						imgData[1] = *gPtr++;
						imgData[2] = *rPtr++;
						imgData[3] = *aPtr++;
						imgData += 4;
					}
				}
				else
				{
					UInt8 tmpByte;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						tmpByte = imgData[0];
						imgData[0] = imgData[2];
						imgData[2] = tmpByte;
						imgData += 4;
					}
				}
			}
			else if (nChannels == 3 && bpp == 48)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *rPtr = planarBuff;
					UInt8 *gPtr = rPtr + imgWidth * imgHeight * 2;
					UInt8 *bPtr = gPtr + imgWidth * imgHeight * 2;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], readInt16(bPtr));
						WriteInt16(&imgData[2], readInt16(gPtr));
						WriteInt16(&imgData[4], readInt16(rPtr));
						bPtr += 2;
						gPtr += 2;
						rPtr += 2;
						imgData += 6;
					}
				}
				else
				{
					Int16 tmpByte;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						tmpByte = readInt16(&imgData[0]);
						WriteInt16(&imgData[0], readInt16(&imgData[4]));
						WriteInt16(&imgData[2], readInt16(&imgData[2]));
						WriteInt16(&imgData[4], tmpByte);
						imgData += 6;
					}
				}
			}
			else if (nChannels == 4 && bpp == 64)
			{
				if (planarConfiguration == 2)
				{
					UInt8 *rPtr = planarBuff;
					UInt8 *gPtr = rPtr + imgWidth * imgHeight * 2;
					UInt8 *bPtr = gPtr + imgWidth * imgHeight * 2;
					UInt8 *aPtr = bPtr + imgWidth * imgHeight * 2;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], readInt16(bPtr));
						WriteInt16(&imgData[2], readInt16(gPtr));
						WriteInt16(&imgData[4], readInt16(rPtr));
						WriteInt16(&imgData[6], readInt16(aPtr));
						bPtr += 2;
						gPtr += 2;
						rPtr += 2;
						aPtr += 2;
						imgData += 8;
					}
				}
				else
				{
					Int16 tmpByte;
					imgData = (UInt8*)img->data;
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						tmpByte = readInt16(&imgData[0]);
						WriteInt16(&imgData[0], readInt16(&imgData[4]));
						WriteInt16(&imgData[2], readInt16(&imgData[2]));
						WriteInt16(&imgData[4], tmpByte);
						WriteInt16(&imgData[6], readInt16(&imgData[6]));
						imgData += 8;
					}
				}
			}
			if (predictor == 2)
			{
				imgData = (UInt8*)img->data;
				if (img->info->pf == Media::PF_PAL_W8)
				{
					i = img->info->dispHeight;
					while (i-- > 0)
					{
						imgData += 1;
						j = img->info->dispWidth - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-1]);
							imgData += 1;
						}
					}
				}
				else if (img->info->pf == Media::PF_W8A8)
				{
					i = img->info->dispHeight;
					while (i-- > 0)
					{
						imgData += 2;
						j = img->info->dispWidth - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-2]);
							imgData[1] = (UInt8)(imgData[1] + imgData[-1]);
							imgData += 2;
						}
					}
				}
				else if (img->info->pf == Media::PF_B8G8R8)
				{
					i = img->info->dispHeight;
					while (i-- > 0)
					{
						imgData += 3;
						j = img->info->dispWidth - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-3]);
							imgData[1] = (UInt8)(imgData[1] + imgData[-2]);
							imgData[2] = (UInt8)(imgData[2] + imgData[-1]);
							imgData += 3;
						}
					}
				}
				else if (img->info->pf == Media::PF_B8G8R8A8)
				{
					i = img->info->dispHeight;
					while (i-- > 0)
					{
						imgData += 4;
						j = img->info->dispWidth - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-4]);
							imgData[1] = (UInt8)(imgData[1] + imgData[-3]);
							imgData[2] = (UInt8)(imgData[2] + imgData[-2]);
							imgData[3] = (UInt8)(imgData[3] + imgData[-1]);
							imgData += 4;
						}
					}
				}
				else
				{
					predictor = 2;
				}
			}

			img->SetEXIFData(exif);
			Double dpi;
			if ((dpi = exif->GetHDPI()) != 0)
			{
				img->info->hdpi = dpi;
			}
			if ((dpi = exif->GetVDPI()) != 0)
			{
				img->info->vdpi = dpi;
			}
			imgList->AddImage(img, 1000);
			if (stripOfsts)
			{
				MemFree(stripOfsts);
			}
			if (stripLengs)
			{
				MemFree(stripLengs);
			}
			if (planarBuff)
			{
				MemFree(planarBuff);
			}
		}
	}

	if (imgList->GetCount() == 1 && targetType != IO::ParserType::ImageList)
	{
		Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		UInt32 srid;
		if (img->exif->GetGeoBounds(img->info->dispWidth, img->info->dispHeight, &srid, &minX, &minY, &maxX, &maxY))
		{
			Map::VectorLayer *lyr;
			Math::VectorImage *vimg;
			Media::SharedImage *simg;
			
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, 0, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, 0, 0, 0, 0));
			img->To32bpp();
			NEW_CLASS(simg, Media::SharedImage(imgList, true));
			NEW_CLASS(vimg, Math::VectorImage(srid, simg, minX, minY, maxX, maxY, false, fd->GetFullName(), 0, 0));
			lyr->AddVector(vimg, 0);
			DEL_CLASS(simg);
			
			return lyr;
		}

		if (fd->IsFullFile())
		{
			UTF8Char fileNameBuff[1024];
			Text::StrConcat(fileNameBuff, fd->GetFullFileName());
			IO::Path::ReplaceExt(fileNameBuff, (const UTF8Char*)"tfw");
			if (IO::Path::GetPathType(fileNameBuff) == IO::Path::PathType::File)
			{
				IO::FileStream *fs;
				IO::StreamReader *reader;
				Text::StringBuilderUTF8 sb;
				Bool valid = true;
				Double xPxSize;
				Double rotY;
				Double rotX;
				Double yPxSize;
				Double xCoord;
				Double yCoord;
				NEW_CLASS(fs, IO::FileStream(fileNameBuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
				NEW_CLASS(reader, IO::StreamReader(fs, 0));
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &xPxSize))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &rotY))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &rotX))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &yPxSize))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &xCoord))
				{
					valid = false;
				}
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 1024) || !Text::StrToDouble(sb.ToString(), &yCoord))
				{
					valid = false;
				}
				DEL_CLASS(reader);
				DEL_CLASS(fs);

				if (valid && rotX == 0 && rotY == 0)
				{
					Map::VectorLayer *lyr;
					Math::VectorImage *vimg;
					Media::SharedImage *simg;
					Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
					
					NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), 0, 0, csys, 0, 0, 0, 0, 0));
					img->To32bpp();
					NEW_CLASS(simg, Media::SharedImage(imgList, true));
					NEW_CLASS(vimg, Math::VectorImage(csys->GetSRID(), simg, xCoord - xPxSize * 0.5, yCoord + yPxSize * (Math::UOSInt2Double(img->info->dispHeight) - 0.5), xCoord + xPxSize * (Math::UOSInt2Double(img->info->dispWidth) - 0.5), yCoord - yPxSize * 0.5, false, fd->GetFullName(), 0, 0));
					lyr->AddVector(vimg, 0);
					DEL_CLASS(simg);
					
					return lyr;
				}
			}
		}
	}
	return imgList;
}

UInt32 Parser::FileParser::TIFFParser::GetUInt(Media::EXIFData *exif, Int32 id)
{
	Media::EXIFData::EXIFItem *item = exif->GetExifItem(id);
	if (item == 0)
		return 0;
	if (item->size != 1)
		return 0;
	if (item->type == Media::EXIFData::ET_UINT16)
		return *exif->GetExifUInt16(id);
	if (item->type == Media::EXIFData::ET_UINT32)
		return *exif->GetExifUInt32(id);
	return 0;
}

UInt32 Parser::FileParser::TIFFParser::GetUInt0(Media::EXIFData *exif, Int32 id)
{
	Media::EXIFData::EXIFItem *item = exif->GetExifItem(id);
	if (item == 0)
		return 0;
	if (item->type == Media::EXIFData::ET_UINT16)
		return *exif->GetExifUInt16(id);
	if (item->type == Media::EXIFData::ET_UINT32)
		return *exif->GetExifUInt32(id);
	return 0;
}

UInt32 Parser::FileParser::TIFFParser::GetUIntSum(Media::EXIFData *exif, Int32 id, UOSInt *nChannels)
{
	Media::EXIFData::EXIFItem *item = exif->GetExifItem(id);
	if (item == 0)
		return 0;
	UInt32 sum = 0;
	UOSInt i = item->size;
	if (nChannels)
	{
		*nChannels = i;
	}
	if (item->type == Media::EXIFData::ET_UINT16)
	{
		UInt16 *val = exif->GetExifUInt16(id);
		while (i-- > 0)
		{
			sum += val[i];
		}
		return sum;
	}
	if (item->type == Media::EXIFData::ET_UINT32)
	{
		UInt32 *val = exif->GetExifUInt32(id);
		while (i-- > 0)
		{
			sum += val[i];
		}
		return sum;
	}
	return 0;
}
