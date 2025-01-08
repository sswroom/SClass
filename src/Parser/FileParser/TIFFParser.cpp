#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/PackBits.h"
#include "Data/Compress/LZWDecompressor.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/FrameInfo.h"
#include "Media/ICCProfile.h"
#include "Media/IImgResizer.h"
#include "Media/ImageList.h"
#include "Media/ImagePreviewTool.h"
#include "Media/StaticImage.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/TIFFParser.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

//https://www.awaresystems.be/imaging/tiff/bigtiff.html

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

void Parser::FileParser::TIFFParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::TIFFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.tif"), CSTR("TIFF File"));
		selector->AddFilter(CSTR("*.tiff"), CSTR("TIFF File"));
	}
}

IO::ParserType Parser::FileParser::TIFFParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::TIFFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	NN<Data::ByteOrder> bo;

	UInt64 nextOfst;
	UOSInt i;

	if (*(Int16*)&hdr[0] == *(Int16*)"MM")
	{
		NEW_CLASSNN(bo, Data::ByteOrderMSB());
	}
	else if (*(Int16*)&hdr[0] == *(Int16*)"II")
	{
		NEW_CLASSNN(bo, Data::ByteOrderLSB());
	}
	else
	{
		return 0;
	}
	NN<Media::ImageList> imgList;
	NN<Media::StaticImage> img;
	Optional<Media::EXIFData> exif;
	NN<Media::EXIFData> nnexif;
	UInt16 fmt = bo->GetUInt16(hdr.Arr() + 2);
	if (fmt == 42)
	{
		nextOfst = bo->GetUInt32(hdr.Arr() + 4);
	}
	else if (fmt == 43 && bo->GetUInt16(hdr.Arr() + 4) == 8 && bo->GetUInt16(hdr.Arr() + 6) == 0) //BigTIFF
	{
		nextOfst = bo->GetUInt64(hdr.Arr() + 8);
	}
	else
	{
		bo.Delete();
		return 0;
	}
	
	NEW_CLASSNN(imgList, Media::ImageList(fd->GetFullName()));
	while (nextOfst)
	{
		if (fmt == 42)
		{
			exif = Media::EXIFData::ParseIFD(fd, nextOfst, bo, nextOfst, 0);
		}
		else
		{
			exif = Media::EXIFData::ParseIFD64(fd, nextOfst, bo, nextOfst, 0);;
		}
		if (!exif.SetTo(nnexif))
		{
			imgList.Delete();
			bo.Delete();
			return 0;
		}

		Bool valid = true;
		Bool processed = false;
		UOSInt j;
		UOSInt k;
		UInt32 compression = 0;
		UInt32 imgWidth = 0;
		UInt32 imgHeight = 0;
		UInt32 bpp = 0;
		UOSInt nChannels = 0;
		UInt32 photometricInterpretation = 0;
		UInt32 planarConfiguration = 1;
		UInt32 sampleFormat = 1;
		UInt64 nStrip = 0;
		UInt32 stripOfst = 0;
		UInt32 stripLeng = 0;
		UInt32 *stripOfsts = 0;
		UInt32 *stripLengs = 0;
		UInt32 storeBPP;
		UInt32 predictor;
		UInt32 rowsPerStrip;
		if ((imgWidth = GetUInt(nnexif, 0x100)) == 0)
			valid = false;
		if ((imgHeight = GetUInt(nnexif, 0x101)) == 0)
			valid = false;
		if ((bpp = GetUIntSum(nnexif, 0x102, nChannels)) == 0)
			valid = false;
		sampleFormat = GetUInt0(nnexif, 0x153);
		if (sampleFormat == 0)
			sampleFormat = 1;
		predictor = GetUInt(nnexif, 0x13d);
		if (predictor == 0)
			predictor = 1;
		compression = GetUInt(nnexif, 0x103);
		photometricInterpretation = GetUInt(nnexif, 0x106);
		planarConfiguration = GetUInt(nnexif, 284);
		rowsPerStrip = GetUInt(nnexif, 278);
		storeBPP = bpp;
		NN<Media::EXIFData::EXIFItem> item;
		Media::PixelFormat pf = Media::PixelFormatGetDef(0, bpp);
		if (!nnexif->GetExifItem(0x111).SetTo(item))
		{
			valid = false;
		}
		else if (item->cnt == 1)
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
				UInt16 *val = nnexif->GetExifUInt16(0x111);
				stripOfsts = MemAlloc(UInt32, item->cnt);
				j = item->cnt;
				while (j-- > 0)
				{
					stripOfsts[j] = val[j];
				}
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripOfsts = MemAlloc(UInt32, item->cnt);
				MemCopyNO(stripOfsts, nnexif->GetExifUInt32(0x111), item->cnt * sizeof(UInt32));
			}
			else
			{
				valid = false;
			}
		}
		if (!nnexif->GetExifItem(0x117).SetTo(item))
		{
			valid = false;
		}
		else if (item->cnt == 1)
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
			nStrip = item->cnt;
			if (item->type == Media::EXIFData::ET_UINT16)
			{
				UInt16 *val = nnexif->GetExifUInt16(0x111);
				stripLengs = MemAlloc(UInt32, item->cnt);
				j = item->cnt;
				while (j-- > 0)
				{
					stripLengs[j] = val[j];
				}
			}
			else if (item->type == Media::EXIFData::ET_UINT32)
			{
				stripLengs = MemAlloc(UInt32, item->cnt);
				MemCopyNO(stripLengs, nnexif->GetExifUInt32(0x117), item->cnt * sizeof(UInt32));
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
				Optional<Media::ImageList> innerImgList = 0;
				NN<Media::ImageList> nnInnerImgList;
				NN<IO::StreamData> jpgFd = fd->GetPartialData(stripOfst, stripLeng);
				NN<Parser::ParserList> parsers;
				if (this->parsers.SetTo(parsers))
				{
					innerImgList = Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(jpgFd, IO::ParserType::ImageList));
				}
				jpgFd.Delete();
				if (innerImgList.SetTo(nnInnerImgList))
				{
					NN<Media::RasterImage> innerImg;
					NN<Media::StaticImage> innerSImg;
					UInt32 imgDelay;
					i = 0;
					j = nnInnerImgList->GetCount();
					while (i < j)
					{
						if (nnInnerImgList->GetImage(i, imgDelay).SetTo(innerImg))
						{
							innerSImg = innerImg->CreateStaticImage();
							if (exif.SetTo(nnexif))
							{
								innerSImg->SetEXIFData(nnexif).Delete();
								exif = 0;
							}
							innerSImg.Delete();
						}
						i++;
					}
					nnInnerImgList.Delete();
					exif.Delete();
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
			exif.Delete();
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
				bo.Delete();
				return imgList;
			}
			else
			{
				imgList.Delete();
				bo.Delete();
				return 0;
			}
		}
		else if (processed)
		{
		}
		else
		{
			Media::ColorProfile color(Media::ColorProfile::CPT_PUNKNOWN);
			if (nnexif->GetExifItem(34675).SetTo(item))
			{
				NN<Media::ICCProfile> icc;
				if (Media::ICCProfile::Parse(Data::ByteArrayR(item->dataBuff.GetOpt<UInt8>().OrNull(), item->cnt)).SetTo(icc))
				{
					icc->GetRedTransferParam(color.GetRTranParam());
					icc->GetGreenTransferParam(color.GetGTranParam());
					icc->GetBlueTransferParam(color.GetBTranParam());
					icc->GetColorPrimaries(color.GetPrimaries());
					color.SetRAWICC(item->dataBuff.GetOpt<UInt8>().OrNull());
					icc.Delete();
				}
			}

			NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(imgWidth, imgHeight), 0, storeBPP, pf, 0, color, Media::ColorProfile::YUVT_UNKNOWN, (bpp == 32 || bpp == 64)?Media::AT_ALPHA:Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
			Data::ByteArray imgData;
			UnsafeArrayOpt<UInt8> planarBuff = 0;
			UnsafeArray<UInt8> nnplanarBuff;
			if (sampleFormat == 3)
			{
				nnplanarBuff = MemAllocArr(UInt8, imgWidth * imgHeight * bpp >> 3);
				imgData = Data::ByteArray(nnplanarBuff, imgWidth * imgHeight * bpp >> 3);
				planarBuff = nnplanarBuff;
			}
			else if (planarConfiguration == 2 && nChannels > 1)
			{
				nnplanarBuff = MemAllocArr(UInt8, imgWidth * imgHeight * bpp >> 3);
				imgData = Data::ByteArray(nnplanarBuff, imgWidth * imgHeight * bpp >> 3);
				planarBuff = nnplanarBuff;
			}
			else
			{
				imgData = img->GetDataArray();
			}
			if (compression == 32773)
			{
				UOSInt maxLeng;
				UOSInt lengLeft;
				UOSInt destSize;
				if (nStrip == 1)
				{
					Data::ByteBuffer compImgData(stripLeng);
					lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
					Data::Compress::PackBits::Decompress(imgData.Arr(), destSize, compImgData.Arr(), lengLeft);
				}
				else
				{
					maxLeng = stripLengs[i = nStrip - 1];
					while (i-- > 0)
					{
						if (stripLengs[i] > maxLeng)
							maxLeng = stripLengs[i];
					}
					Data::ByteBuffer compImgData(maxLeng);

//					OSInt dbpl = imgWidth * bpp >> 3;
//					OSInt decBpl = (((imgWidth * bpp) & ~7) + 7)/ 8;
					i = 0;
					while (i < nStrip)
					{
						//fd->GetRealData(stripOfsts[i], stripLengs[i], imgData);
						//imgData += stripLengs[i];

						lengLeft = fd->GetRealData(stripOfsts[i], stripLengs[i], compImgData);
						Data::Compress::PackBits::Decompress(imgData.Arr(), destSize, compImgData.Arr(), lengLeft);
						imgData = imgData.SubArray(destSize);

						i++;
					}
				}
			}
			else if (compression == 5 || compression == 8)
			{
				UOSInt maxLeng;
				UOSInt lengLeft;
				UOSInt destSize;
				if (compression == 5)
				{
					Data::Compress::LZWDecompressor lzw;
					if (nStrip == 1)
					{
						Data::ByteBuffer compImgData(stripLeng);
						lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
						lzw.Decompress(imgData, destSize, compImgData.SubArray(0, lengLeft));
					}
					else
					{
						maxLeng = stripLengs[i = nStrip - 1];
						while (i-- > 0)
						{
							if (stripLengs[i] > maxLeng)
								maxLeng = stripLengs[i];
						}
						Data::ByteBuffer compImgData(maxLeng);

						UOSInt dbpl = ((imgWidth * bpp) + 7) >> 3;
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
							lzw.Decompress(imgData, destSize, compImgData.SubArray(0, lengLeft));
							if (destSize != decSize)
							{
								destSize = decSize;
							}
							imgData = imgData.SubArray(destSize);

							i++;
						}
					}
				}
				else if (compression == 8)
				{
					Data::Compress::Inflate inflate(false);
					if (nStrip == 1)
					{
						Data::ByteBuffer compImgData(stripLeng);
						lengLeft = fd->GetRealData(stripOfst, stripLeng, compImgData);
						inflate.Decompress(imgData, destSize, compImgData.SubArray(0, lengLeft));
					}
					else
					{
						maxLeng = stripLengs[i = nStrip - 1];
						while (i-- > 0)
						{
							if (stripLengs[i] > maxLeng)
								maxLeng = stripLengs[i];
						}
						Data::ByteBuffer compImgData(maxLeng);

//						OSInt dbpl = imgWidth * bpp >> 3;
//						OSInt decBpl = (((imgWidth * bpp) & ~7) + 7)/ 8;
						i = 0;
						while (i < nStrip)
						{
							//fd->GetRealData(stripOfsts[i], stripLengs[i], imgData);
							//imgData += stripLengs[i];

							lengLeft = fd->GetRealData(stripOfsts[i], stripLengs[i], compImgData);
							inflate.Decompress(imgData, destSize, compImgData.SubArray(0, lengLeft));
							imgData = imgData.SubArray(destSize);

							i++;
						}
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
						imgData = imgData.SubArray(stripLengs[i]);
						i++;
					}
				}
			}

			if (sampleFormat == 3 && predictor == 3 && planarBuff.SetTo(nnplanarBuff))
			{
				UnsafeArray<UInt8> tmpBuff;
				UnsafeArray<UInt8> tmpPtr;
				UnsafeArray<UInt8> tmpPtr2;
				Data::ByteArray tmpArray;
				UInt8 lastPx[4];
				UOSInt bytePerChannels = (bpp >> 3) / nChannels;
				UOSInt l;
				if (planarConfiguration == 2)
				{
					tmpBuff = MemAllocArr(UInt8, bytePerChannels * imgWidth);
					imgData = Data::ByteArray(nnplanarBuff, imgWidth * imgHeight * bpp >> 3);
					if (bo->IsBigEndian())
					{
						i = nChannels;
						while (i-- > 0)
						{
							j = imgHeight;
							while (j-- > 0)
							{
								tmpPtr = imgData.Arr();
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
								imgData.CopyFrom(0, Data::ByteArrayR(tmpBuff, bytePerChannels * imgWidth));
								imgData = imgData.SubArray(bytePerChannels * imgWidth);
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
								tmpPtr = imgData.Arr();
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
								imgData.CopyFrom(0, Data::ByteArrayR(tmpBuff, bytePerChannels * imgWidth));
								imgData = imgData.SubArray(bytePerChannels * imgWidth);
							}
						}
					}
					MemFreeArr(tmpBuff);
				}
				else
				{
					tmpBuff = MemAllocArr(UInt8, bytePerChannels * imgWidth * nChannels);
					/////////////////////////////////////////////////////
					if (bo->IsBigEndian())
					{
						imgData = Data::ByteArray(nnplanarBuff, imgWidth * imgHeight * bpp >> 3);
						i = imgHeight;
						while (i-- > 0)
						{
							tmpArray = imgData;
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
										lastPx[l] = (UInt8)(lastPx[l] + tmpArray[0]);
										tmpArray++;
										tmpPtr2[0] = lastPx[l];
										tmpPtr2 += bytePerChannels;
									}
								}
							}
							imgData.CopyFrom(0, Data::ByteArrayR(tmpBuff, bytePerChannels * imgWidth * nChannels));
							imgData = tmpArray;
						}
					}
					else
					{
						imgData = Data::ByteArray(nnplanarBuff, imgWidth * imgHeight * bpp >> 3);
						i = imgHeight;
						while (i-- > 0)
						{
							tmpArray = imgData;
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
										lastPx[l] = (UInt8)(lastPx[l] + tmpArray[0]);
										tmpArray++;
										tmpPtr2[0] = lastPx[l];
										tmpPtr2 += bytePerChannels;
									}
								}
							}
							imgData.CopyFrom(0, Data::ByteArrayR(tmpBuff, bytePerChannels * imgWidth * nChannels));
							imgData = tmpArray;
						}
					}
					MemFreeArr(tmpBuff);
				}
			}

			if (sampleFormat == 3 && planarBuff.SetTo(nnplanarBuff))
			{
				if (nChannels == 1)
				{
					if (bpp == 16)
					{
						UnsafeArray<UInt8> srcPtr = nnplanarBuff;
						imgData = img->GetDataArray();
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], bo->GetFloat16(srcPtr + 0));
							srcPtr += 2;
							imgData += 4;
						}
					}
					else if (bpp == 24)
					{
						UnsafeArray<UInt8> srcPtr = nnplanarBuff;
						imgData = img->GetDataArray();
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], bo->GetFloat24(srcPtr + 0));
							srcPtr += 3;
							imgData += 4;
						}
					}
					else if (bpp == 32)
					{
						UnsafeArray<UInt8> srcPtr = nnplanarBuff;
						imgData = img->GetDataArray();
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteFloat(&imgData[0], bo->GetFloat32(srcPtr + 0));
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
							UnsafeArray<UInt8> wPtr = nnplanarBuff;
							UnsafeArray<UInt8> aPtr = wPtr + imgWidth * imgHeight * 2;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(wPtr));
								WriteFloat(&imgData[4], bo->GetFloat16(aPtr));
								wPtr += 2;
								aPtr += 2;
								imgData += 8;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();;
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(srcPtr + 0));
								WriteFloat(&imgData[4], bo->GetFloat16(srcPtr + 2));
								srcPtr += 4;
								imgData += 8;
							}
						}
					}
					else if (bpp == 48)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> wPtr = nnplanarBuff;
							UnsafeArray<UInt8> aPtr = wPtr + imgWidth * imgHeight * 3;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(wPtr));
								WriteFloat(&imgData[4], bo->GetFloat24(aPtr));
								wPtr += 3;
								aPtr += 3;
								imgData += 8;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(srcPtr + 0));
								WriteFloat(&imgData[4], bo->GetFloat24(srcPtr + 3));
								srcPtr += 6;
								imgData += 8;
							}
						}
					}
					else if (bpp == 64)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> wPtr = nnplanarBuff;
							UnsafeArray<UInt8> aPtr = wPtr + imgWidth * imgHeight * 4;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(wPtr));
								WriteFloat(&imgData[4], bo->GetFloat32(aPtr));
								wPtr += 4;
								aPtr += 4;
								imgData += 8;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(srcPtr + 0));
								WriteFloat(&imgData[4], bo->GetFloat32(srcPtr + 4));
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
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 2;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 2;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat16(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat16(rPtr));
								rPtr += 2;
								gPtr += 2;
								bPtr += 2;
								imgData += 12;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(srcPtr + 4));
								WriteFloat(&imgData[4], bo->GetFloat16(srcPtr + 2));
								WriteFloat(&imgData[8], bo->GetFloat16(srcPtr + 0));
								srcPtr += 6;
								imgData += 12;
							}
						}
					}
					else if (bpp == 72)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 3;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 3;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat24(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat24(rPtr));
								rPtr += 3;
								gPtr += 3;
								bPtr += 3;
								imgData += 12;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(srcPtr + 6));
								WriteFloat(&imgData[4], bo->GetFloat24(srcPtr + 3));
								WriteFloat(&imgData[8], bo->GetFloat24(srcPtr + 0));
								srcPtr += 9;
								imgData += 12;
							}
						}
					}
					else if (bpp == 96)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 4;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 4;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat32(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat32(rPtr));
								rPtr += 4;
								gPtr += 4;
								bPtr += 4;
								imgData += 12;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(srcPtr + 8));
								WriteFloat(&imgData[4], bo->GetFloat32(srcPtr + 4));
								WriteFloat(&imgData[8], bo->GetFloat32(srcPtr + 0));
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
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 2;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 2;
							UnsafeArray<UInt8> aPtr = bPtr + imgWidth * imgHeight * 2;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat16(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat16(rPtr));
								WriteFloat(&imgData[12], bo->GetFloat16(aPtr));
								rPtr += 2;
								gPtr += 2;
								bPtr += 2;
								aPtr += 2;
								imgData += 16;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat16(srcPtr + 4));
								WriteFloat(&imgData[4], bo->GetFloat16(srcPtr + 2));
								WriteFloat(&imgData[8], bo->GetFloat16(srcPtr + 0));
								WriteFloat(&imgData[12], bo->GetFloat16(srcPtr + 6));
								srcPtr += 8;
								imgData += 16;
							}
						}
					}
					else if (bpp == 96)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 3;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 3;
							UnsafeArray<UInt8> aPtr = bPtr + imgWidth * imgHeight * 3;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat24(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat24(rPtr));
								WriteFloat(&imgData[12], bo->GetFloat24(aPtr));
								rPtr += 3;
								gPtr += 3;
								bPtr += 3;
								aPtr += 3;
								imgData += 16;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat24(srcPtr + 6));
								WriteFloat(&imgData[4], bo->GetFloat24(srcPtr + 3));
								WriteFloat(&imgData[8], bo->GetFloat24(srcPtr + 0));
								WriteFloat(&imgData[12], bo->GetFloat24(srcPtr + 9));
								srcPtr += 12;
								imgData += 16;
							}
						}
					}
					else if (bpp == 128)
					{
						if (planarConfiguration == 2)
						{
							UnsafeArray<UInt8> rPtr = nnplanarBuff;
							UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 4;
							UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 4;
							UnsafeArray<UInt8> aPtr = bPtr + imgWidth * imgHeight * 4;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(bPtr));
								WriteFloat(&imgData[4], bo->GetFloat32(gPtr));
								WriteFloat(&imgData[8], bo->GetFloat32(rPtr));
								WriteFloat(&imgData[12], bo->GetFloat32(aPtr));
								rPtr += 4;
								gPtr += 4;
								bPtr += 4;
								aPtr += 4;
								imgData += 16;
							}
						}
						else
						{
							UnsafeArray<UInt8> srcPtr = nnplanarBuff;
							imgData = img->GetDataArray();
							i = imgWidth * imgHeight;
							while (i-- > 0)
							{
								WriteFloat(&imgData[0], bo->GetFloat32(srcPtr + 8));
								WriteFloat(&imgData[4], bo->GetFloat32(srcPtr + 4));
								WriteFloat(&imgData[8], bo->GetFloat32(srcPtr + 0));
								WriteFloat(&imgData[12], bo->GetFloat32(srcPtr + 12));
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
					UInt16 *pal = nnexif->GetExifUInt16(0x140);
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

					imgData = img->GetDataArray();
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
				imgData = img->GetDataArray();
				if (bo->IsBigEndian())
				{
					i = imgWidth * imgHeight;
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], ReadMInt16(&imgData[0]));
						imgData += 2;
					}
				}
			}
			else if (nChannels == 2 && bpp == 16)
			{
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> wPtr = nnplanarBuff;
					UnsafeArray<UInt8> aPtr = wPtr + imgWidth * imgHeight;
					imgData = img->GetDataArray();
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
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> wPtr = nnplanarBuff;
					UnsafeArray<UInt8> aPtr = wPtr + imgWidth * imgHeight * 2;
					imgData = img->GetDataArray();
					i = imgWidth * imgHeight;
					if (bo->IsBigEndian())
					{
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadMInt16(wPtr));
							WriteInt16(&imgData[2], ReadMInt16(aPtr));
							wPtr += 2;
							aPtr += 2;
							imgData += 4;
						}
					}
					else
					{
					while (i-- > 0)
					{
						WriteInt16(&imgData[0], ReadInt16(wPtr.Ptr()));
						WriteInt16(&imgData[2], ReadInt16(aPtr.Ptr()));
						wPtr += 2;
						aPtr += 2;
						imgData += 4;
					}
					}
				}
				else
				{
					if (bo->IsBigEndian())
					{
						imgData = img->GetDataArray();
						i = imgWidth * imgHeight;
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadMInt16(&imgData[0]));
							WriteInt16(&imgData[2], ReadMInt16(&imgData[2]));
							imgData += 4;
						}
					}
				}
			}
			else if (nChannels == 3 && bpp == 24)
			{
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> rPtr = nnplanarBuff;
					UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight;
					UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight;
					imgData = img->GetDataArray();
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
					imgData = img->GetDataArray();
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
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> rPtr = nnplanarBuff;
					UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight;
					UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight;
					UnsafeArray<UInt8> aPtr = bPtr + imgWidth * imgHeight;
					imgData = img->GetDataArray();
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
					imgData = img->GetDataArray();
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
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> rPtr = nnplanarBuff;
					UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 2;
					UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 2;
					imgData = img->GetDataArray();
					i = imgWidth * imgHeight;
					if (bo->IsBigEndian())
					{
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadMInt16(bPtr));
							WriteInt16(&imgData[2], ReadMInt16(gPtr));
							WriteInt16(&imgData[4], ReadMInt16(rPtr));
							bPtr += 2;
							gPtr += 2;
							rPtr += 2;
							imgData += 6;
						}
					}
					else
					{
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadInt16(bPtr.Ptr()));
							WriteInt16(&imgData[2], ReadInt16(gPtr.Ptr()));
							WriteInt16(&imgData[4], ReadInt16(rPtr.Ptr()));
							bPtr += 2;
							gPtr += 2;
							rPtr += 2;
							imgData += 6;
						}
					}
				}
				else
				{
					Int16 tmpByte;
					imgData = img->GetDataArray();
					i = imgWidth * imgHeight;
					if (bo->IsBigEndian())
					{
						while (i-- > 0)
						{
							tmpByte = ReadMInt16(&imgData[0]);
							WriteInt16(&imgData[0], ReadMInt16(&imgData[4]));
							WriteInt16(&imgData[2], ReadMInt16(&imgData[2]));
							WriteInt16(&imgData[4], tmpByte);
							imgData += 6;
						}
					}
					else
					{
						while (i-- > 0)
						{
							tmpByte = ReadInt16(&imgData[0]);
							WriteInt16(&imgData[0], ReadInt16(&imgData[4]));
							WriteInt16(&imgData[4], tmpByte);
							imgData += 6;
						}
					}
				}
			}
			else if (nChannels == 4 && bpp == 64)
			{
				if (planarConfiguration == 2 && planarBuff.SetTo(nnplanarBuff))
				{
					UnsafeArray<UInt8> rPtr = nnplanarBuff;
					UnsafeArray<UInt8> gPtr = rPtr + imgWidth * imgHeight * 2;
					UnsafeArray<UInt8> bPtr = gPtr + imgWidth * imgHeight * 2;
					UnsafeArray<UInt8> aPtr = bPtr + imgWidth * imgHeight * 2;
					imgData = img->GetDataArray();
					i = imgWidth * imgHeight;
					if (bo->IsBigEndian())
					{
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadMInt16(bPtr));
							WriteInt16(&imgData[2], ReadMInt16(gPtr));
							WriteInt16(&imgData[4], ReadMInt16(rPtr));
							WriteInt16(&imgData[6], ReadMInt16(aPtr));
							bPtr += 2;
							gPtr += 2;
							rPtr += 2;
							aPtr += 2;
							imgData += 8;
						}
					}
					else
					{
						while (i-- > 0)
						{
							WriteInt16(&imgData[0], ReadInt16(bPtr.Ptr()));
							WriteInt16(&imgData[2], ReadInt16(gPtr.Ptr()));
							WriteInt16(&imgData[4], ReadInt16(rPtr.Ptr()));
							WriteInt16(&imgData[6], ReadInt16(aPtr.Ptr()));
							bPtr += 2;
							gPtr += 2;
							rPtr += 2;
							aPtr += 2;
							imgData += 8;
						}
					}
				}
				else
				{
					Int16 tmpByte;
					imgData = img->GetDataArray();
					i = imgWidth * imgHeight;
					if (bo->IsBigEndian())
					{
						while (i-- > 0)
						{
							tmpByte = ReadMInt16(&imgData[0]);
							WriteInt16(&imgData[0], ReadMInt16(&imgData[4]));
							WriteInt16(&imgData[2], ReadMInt16(&imgData[2]));
							WriteInt16(&imgData[4], tmpByte);
							WriteInt16(&imgData[6], ReadMInt16(&imgData[6]));
							imgData += 8;
						}
					}
					else
					{
						while (i-- > 0)
						{
							tmpByte = ReadInt16(&imgData[0]);
							WriteInt16(&imgData[0], ReadInt16(&imgData[4]));
							WriteInt16(&imgData[4], tmpByte);
							imgData += 8;
						}
					}
				}
			}
			if (predictor == 2)
			{
				imgData = img->GetDataArray();
				if (img->info.pf == Media::PF_PAL_W8)
				{
					i = img->info.dispSize.y;
					while (i-- > 0)
					{
						imgData += 1;
						j = img->info.dispSize.x - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-1]);
							imgData += 1;
						}
					}
				}
				else if (img->info.pf == Media::PF_W8A8)
				{
					i = img->info.dispSize.y;
					while (i-- > 0)
					{
						imgData += 2;
						j = img->info.dispSize.x - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-2]);
							imgData[1] = (UInt8)(imgData[1] + imgData[-1]);
							imgData += 2;
						}
					}
				}
				else if (img->info.pf == Media::PF_B8G8R8)
				{
					i = img->info.dispSize.y;
					while (i-- > 0)
					{
						imgData += 3;
						j = img->info.dispSize.x - 1;
						while (j-- > 0)
						{
							imgData[0] = (UInt8)(imgData[0] + imgData[-3]);
							imgData[1] = (UInt8)(imgData[1] + imgData[-2]);
							imgData[2] = (UInt8)(imgData[2] + imgData[-1]);
							imgData += 3;
						}
					}
				}
				else if (img->info.pf == Media::PF_B8G8R8A8)
				{
					i = img->info.dispSize.y;
					while (i-- > 0)
					{
						imgData += 4;
						j = img->info.dispSize.x - 1;
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
			if ((dpi = nnexif->GetHDPI()) != 0)
			{
				img->info.hdpi = dpi;
			}
			if ((dpi = nnexif->GetVDPI()) != 0)
			{
				img->info.vdpi = dpi;
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
			if (planarBuff.SetTo(nnplanarBuff))
			{
				MemFreeArr(nnplanarBuff);
			}
		}
	}

	if (imgList->GetCount() == 1 && targetType != IO::ParserType::ImageList && Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
	{
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		UInt32 srid;
		if (img->exif.SetTo(nnexif) && nnexif->GetGeoBounds(img->info.dispSize, srid, minX, minY, maxX, maxY))
		{
			Map::VectorLayer *lyr;
			NN<Math::Geometry::VectorImage> vimg;
			NN<Media::SharedImage> simg;
			
			NN<Math::CoordinateSystem> csys;
			if (srid == 0)
			{
				csys = Math::CoordinateSystemManager::CreateWGS84Csys();
			}
			else
			{
				csys = Math::CoordinateSystemManager::SRCreateCSysOrDef(srid);
			}
			NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, 0));
			img->To32bpp();
			Data::ArrayListNN<Media::StaticImage> prevList;
			Media::ImagePreviewTool::CreatePreviews(imgList, prevList, 640);
			NEW_CLASSNN(simg, Media::SharedImage(imgList, prevList));
			NEW_CLASSNN(vimg, Math::Geometry::VectorImage(srid, simg, Math::Coord2DDbl(minX, minY), Math::Coord2DDbl(maxX, maxY), false, fd->GetFullName().Ptr(), 0, 0));
			lyr->AddVector(vimg, (Text::String**)0);
			simg.Delete();
			bo.Delete();
			return lyr;
		}

		if (fd->IsFullFile())
		{
			UTF8Char fileNameBuff[1024];
			UnsafeArray<UTF8Char> sptr;
			fd->GetFullFileName()->ConcatTo(fileNameBuff);
			sptr = IO::Path::ReplaceExt(UARR(fileNameBuff), UTF8STRC("tfw"));
			if (IO::Path::GetPathType(CSTRP(fileNameBuff, sptr)) == IO::Path::PathType::File)
			{
				Text::StringBuilderUTF8 sb;
				Bool valid = true;
				Double xPxSize;
				Double rotY;
				Double rotX;
				Double yPxSize;
				Double xCoord;
				Double yCoord;
				{
					IO::FileStream fs(CSTRP(fileNameBuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential);
					IO::StreamReader reader(fs, 0);
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotY))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), rotX))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yPxSize))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), xCoord))
					{
						valid = false;
					}
					sb.ClearStr();
					if (!reader.ReadLine(sb, 1024) || !Text::StrToDouble(sb.ToString(), yCoord))
					{
						valid = false;
					}
				}

				if (valid && rotX == 0 && rotY == 0)
				{
					Map::VectorLayer *lyr;
					NN<Math::Geometry::VectorImage> vimg;
					NN<Media::SharedImage> simg;
					NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateWGS84Csys();
					
					NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, fd->GetFullName(), csys, 0));
					img->To32bpp();
					Data::ArrayListNN<Media::StaticImage> prevList;
					Media::ImagePreviewTool::CreatePreviews(imgList, prevList, 640);
					NEW_CLASSNN(simg, Media::SharedImage(imgList, prevList));
					NEW_CLASSNN(vimg, Math::Geometry::VectorImage(csys->GetSRID(), simg, Math::Coord2DDbl(xCoord - xPxSize * 0.5, yCoord + yPxSize * (UOSInt2Double(img->info.dispSize.y) - 0.5)), Math::Coord2DDbl(xCoord + xPxSize * (UOSInt2Double(img->info.dispSize.x) - 0.5), yCoord - yPxSize * 0.5), false, fd->GetFullName().Ptr(), 0, 0));
					lyr->AddVector(vimg, (Text::String**)0);
					simg.Delete();
					
					bo.Delete();
					return lyr;
				}
			}
		}
	}
	bo.Delete();
	return imgList;
}

UInt32 Parser::FileParser::TIFFParser::GetUInt(NN<Media::EXIFData> exif, UInt32 id)
{
	NN<Media::EXIFData::EXIFItem> item;
	if (!exif->GetExifItem(id).SetTo(item))
		return 0;
	if (item->cnt != 1)
		return 0;
	if (item->type == Media::EXIFData::ET_UINT16)
		return *exif->GetExifUInt16(id);
	if (item->type == Media::EXIFData::ET_UINT32)
		return *exif->GetExifUInt32(id);
	return 0;
}

UInt32 Parser::FileParser::TIFFParser::GetUInt0(NN<Media::EXIFData> exif, UInt32 id)
{
	NN<Media::EXIFData::EXIFItem> item;
	if (!exif->GetExifItem(id).SetTo(item))
		return 0;
	if (item->type == Media::EXIFData::ET_UINT16)
		return *exif->GetExifUInt16(id);
	if (item->type == Media::EXIFData::ET_UINT32)
		return *exif->GetExifUInt32(id);
	return 0;
}

UInt32 Parser::FileParser::TIFFParser::GetUIntSum(NN<Media::EXIFData> exif, UInt32 id, OptOut<UOSInt> nChannels)
{
	NN<Media::EXIFData::EXIFItem> item;
	if (!exif->GetExifItem(id).SetTo(item))
		return 0;
	UInt32 sum = 0;
	UOSInt i = item->cnt;
	nChannels.Set(i);
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
