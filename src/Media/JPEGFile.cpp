#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteOrderLSB.h"
#include "Data/ByteOrderMSB.h"
#include "Data/ByteTool.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Media/JPEGFile.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

//http://stackoverflow.com/questions/662565/how-to-create-huffman-tree-from-ffc4-dht-header-in-jpeg-file
//http://u88.n24.queensu.ca/exiftool/forum/index.php?topic=4898.0 FLIR

Bool Media::JPEGFile::ParseJPEGHeader(NotNullPtr<IO::StreamData> fd, Media::RasterImage *img, NotNullPtr<Media::ImageList> imgList, Parser::ParserList *parsers)
{
	UInt64 ofst;
	UInt64 nextOfst;
	UInt32 j;
	UInt8 buff[18];
	Bool ret = false;
	IO::MemoryStream *flirMstm = 0;
	UInt8 flirMaxSegm;
	UInt8 flirCurrSegm = 0;

	if (fd->GetRealData(0, 2, BYTEARR(buff)) != 2)
		return false;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return false;
	ofst = 2;
	while (true)
	{
		if (fd->GetRealData(ofst, 4, BYTEARR(buff)) != 4)
		{
			ret = false;
			break;
		}
		if (buff[0] != 0xff)
		{
			ret = false;
			break;
		}
		if (buff[1] == 0xdb)
		{
			ret = true;
			break;
		}

		j = (UInt32)((buff[2] << 8) | buff[3]) - 2;
		if (buff[1] == 0xe1)
		{
			fd->GetRealData(ofst + 4, 14, BYTEARR(buff));
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				NotNullPtr<Data::ByteOrder> bo;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					NEW_CLASSNN(bo, Data::ByteOrderLSB());
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					NEW_CLASSNN(bo, Data::ByteOrderMSB());
				}
				else
				{
					ret = false;
					break;
				}
				if (bo->GetUInt16(&buff[8]) != 42)
				{
					ret = false;
					bo.Delete();
					break;
				}
				if (bo->GetUInt32(&buff[10]) != 8)
				{
					ret = false;
					bo.Delete();
					break;
				}
				img->exif.Delete();
				img->exif = Media::EXIFData::ParseIFD(fd, ofst + 18, bo, nextOfst, ofst + 10);
				bo.Delete();
				ofst += j + 4;
			}
			else if (*(Int32*)buff == *(Int32*)"FLIR")
			{
				if (buff[4] == 0 && buff[5] == 1)
				{
					if (flirMstm == 0 && buff[6] == 0)
					{
						flirMaxSegm = buff[7];
						NEW_CLASS(flirMstm, IO::MemoryStream());
						flirCurrSegm = buff[6];
						Data::ByteBuffer tagBuff(j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
					}
					else if (flirMstm && buff[6] == (flirCurrSegm + 1))
					{
						flirCurrSegm = (UInt8)(flirCurrSegm + 1);
						Data::ByteBuffer tagBuff(j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
					}
				}
				ofst += j + 4;
			}
			else
			{
				ofst += j + 4;
			}
		}
		else if (buff[1] == 0xe2)
		{
			Data::ByteBuffer tagBuff(j);
			fd->GetRealData(ofst + 4, j, tagBuff);
			if (Text::StrStartsWithC(tagBuff.Ptr(), j, UTF8STRC("ICC_PROFILE")))
			{
				NotNullPtr<Media::ICCProfile> icc;
				if (Media::ICCProfile::Parse(tagBuff.SubArray(14, j - 14)).SetTo(icc))
				{
					icc->SetToColorProfile(img->info.color);
					icc.Delete();
				}
			}
			ofst += j + 4;
		}
		else
		{
			ofst += j + 4;
		}
	}

	if (flirMstm)
	{
		if (flirMaxSegm == flirCurrSegm)
		{
			UOSInt i;
			UOSInt k;
			UInt32 blkOfst;
			UInt32 blkSize;
//			UInt32 blkType;
			UInt32 delay;
			Data::ByteArray tagBuff = flirMstm->GetArray();
			if (tagBuff[0] == 'F' && tagBuff[1] == 'F' && tagBuff[2] == 'F')
			{
				j = 0x44;
				while (true)
				{
//					blkType = ReadMUInt32(&tagBuff[j]);
					blkOfst = ReadMUInt32(&tagBuff[j + 8]);
					blkSize = ReadMUInt32(&tagBuff[j + 12]);
					if (blkSize == 0)
						break;

					if (tagBuff[blkOfst] == 3) // JPG
					{
						Media::ImageList *innerImgList;
						Media::RasterImage *innerImg;
						{
							IO::StmData::MemoryDataRef mfd(tagBuff.SubArray(blkOfst + 13, blkSize - 32));
							innerImgList = (Media::ImageList*)parsers->ParseFileType(mfd, IO::ParserType::ImageList);
						}
						if (innerImgList)
						{
							k = innerImgList->GetCount();
							i = 0;
							while (i < k)
							{
								innerImg = innerImgList->GetImage(i, delay);
								imgList->AddImage(innerImg->CreateStaticImage(), delay);
								i++;
							}
							DEL_CLASS(innerImgList);
						}
					}
					else if (tagBuff[blkOfst] == 2)
					{
						UInt32 innerW = ReadUInt16(&tagBuff[blkOfst + 2]);
						UInt32 innerH = ReadUInt16(&tagBuff[blkOfst + 4]);
						if (blkSize == innerW * innerH * 2 + 32)
						{
							imgList->SetThermoImage(Math::Size2D<UOSInt>(innerW, innerH), 16, &tagBuff[blkOfst + 32], 0.95, 0, 0, Media::ImageList::TT_FLIR);
						}
						else if (tagBuff[blkOfst + 32] == 0x89 && tagBuff[blkOfst + 33] == 0x50 && tagBuff[blkOfst + 34] == 0x4e && tagBuff[blkOfst + 35] == 0x47)
						{
							Media::ImageList *innerImgList;
							Media::RasterImage *innerImg;
							NotNullPtr<Media::StaticImage> stImg;
							{
								IO::StmData::MemoryDataRef mfd(&tagBuff[blkOfst + 32], blkSize - 32);
								innerImgList = (Media::ImageList*)parsers->ParseFileType(mfd, IO::ParserType::ImageList);
							}
							if (innerImgList)
							{
								k = innerImgList->GetCount();
								i = 0;
								while (i < k)
								{
									innerImg = innerImgList->GetImage(i, delay);
									stImg = innerImg->CreateStaticImage();
									if (stImg->info.pf == Media::PF_LE_W16)
									{
										UInt8 *imgPtr = stImg->data;
										UOSInt pixelCnt = stImg->info.dispSize.CalcArea();
										while (pixelCnt-- > 0)
										{
											WriteInt16(imgPtr, ReadMInt16(imgPtr));
											imgPtr += 2;
										}
										imgList->SetThermoImage(stImg->info.dispSize, 16, stImg->data, 0.95, 0, 0, Media::ImageList::TT_FLIR);
									}
									imgList->AddImage(stImg, delay);
									i++;
								}
								DEL_CLASS(innerImgList);
							}
						}
					}
					else if (tagBuff[blkOfst] == 0xe0) //Palette
					{
					}

					j += 32;
				}
			}
		}
		DEL_CLASS(flirMstm);
		flirMstm = 0;
	}
	return ret;
}

Optional<Media::EXIFData> Media::JPEGFile::ParseJPEGExif(NotNullPtr<IO::StreamData> fd)
{
	UInt64 ofst;
	UInt64 nextOfst;
	UInt32 j;
	UInt8 buff[18];
	if (fd->GetRealData(0, 2, BYTEARR(buff)) != 2)
		return 0;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return 0;
	ofst = 2;
	while (true)
	{
		if (fd->GetRealData(ofst, 4, BYTEARR(buff)) != 4)
			return 0;
		if (buff[0] != 0xff)
			return 0;
		if (buff[1] == 0xdb)
			return 0;

		j = (UInt32)((buff[2] << 8) | buff[3]) - 2;
		if (buff[1] == 0xe1)
		{
			fd->GetRealData(ofst + 4, 14, BYTEARR(buff));
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				NotNullPtr<Data::ByteOrder> bo;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					NEW_CLASSNN(bo, Data::ByteOrderLSB());
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					NEW_CLASSNN(bo, Data::ByteOrderMSB());
				}
				else
				{
					return 0;
				}
				if (bo->GetUInt16(&buff[8]) != 42)
				{
					bo.Delete();
					return 0;
				}
				if (bo->GetUInt32(&buff[10]) != 8)
				{
					bo.Delete();
					return 0;
				}
				Optional<Media::EXIFData> exif = Media::EXIFData::ParseIFD(fd, ofst + 18, bo, nextOfst, ofst + 10);
				bo.Delete();
				if (!exif.IsNull())
					return exif;
				ofst += j + 4;
			}
			else
			{
				ofst += j + 4;
			}
		}
		else
		{
			ofst += j + 4;
		}
	}
}

Bool Media::JPEGFile::ParseJPEGHeaders(NotNullPtr<IO::StreamData> fd, OutParam<Optional<Media::EXIFData>> exif, OutParam<Optional<Text::XMLDocument>> xmf, OutParam<Optional<Media::ICCProfile>> icc, OutParam<UInt32> width, OutParam<UInt32> height)
{
	UInt64 ofst;
	UInt64 nextOfst;
	UInt32 j;
	UInt8 buff[30];
	if (fd->GetRealData(0, 2, BYTEARR(buff)) != 2)
		return false;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return false;

	exif.Set(0);
	xmf.Set(0);
	icc.Set(0);
	width.Set(0);
	height.Set(0);

	ofst = 2;
	while (ofst < fd->GetDataSize())
	{
		if (fd->GetRealData(ofst, 4, BYTEARR(buff)) != 4)
			return false;
		if (buff[0] != 0xff)
			return false;

		j = (UInt32)((buff[2] << 8) | buff[3]) - 2;
		switch (buff[1])
		{
		case 0xe1: //APP1
			fd->GetRealData(ofst + 4, 30, BYTEARR(buff));
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				NotNullPtr<Data::ByteOrder> bo;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					NEW_CLASSNN(bo, Data::ByteOrderLSB());
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					NEW_CLASSNN(bo, Data::ByteOrderMSB());
				}
				else
				{
					return false;
				}
				if (bo->GetUInt16(&buff[8]) != 42)
				{
					bo.Delete();
					return false;
				}
				if (bo->GetUInt32(&buff[10]) != 8)
				{
					bo.Delete();
					return false;
				}
				exif.Set(Media::EXIFData::ParseIFD(fd, ofst + 18, bo, nextOfst, ofst + 10));
				bo.Delete();
				ofst += j + 4;
			}
			else if (buff[28] == 0 && Text::StrEquals((Char*)buff, "http://ns.adobe.com/xap/1.0/"))
			{
				Text::EncodingFactory encFact;
				Data::ByteBuffer tagBuff(j - 29);
				fd->GetRealData(ofst + 33, j - 29, tagBuff);
				NotNullPtr<Text::XMLDocument> doc;
				NEW_CLASSNN(doc, Text::XMLDocument());
				if (doc->ParseBuff(encFact, tagBuff.Ptr(), j - 29))
				{
					xmf.Set(doc);
				}
				else
				{
					doc.Delete();
				}
				ofst += j + 4;
			}
			else
			{
				ofst += j + 4;
			}
			break;
		case 0xe2: //APP2
		{
			Data::ByteBuffer tagBuff(j);
			fd->GetRealData(ofst + 4, j, tagBuff);
			if (Text::StrStartsWithC(tagBuff.Ptr(), j, UTF8STRC("ICC_PROFILE")) == 0)
			{
				NotNullPtr<Media::ICCProfile> newIcc;
				if (Media::ICCProfile::Parse(Data::ByteArrayR(&tagBuff[14], j - 14)).SetTo(newIcc))
				{
					icc.Set(newIcc);
				}
			}
			ofst += j + 4;
			break;
		}
		case 0xe0: //APP0
		case 0xe3: //APP3
		case 0xe4: //APP4
		case 0xe5: //APP5
		case 0xe6: //APP6
		case 0xe7: //APP7
		case 0xe8: //APP8
		case 0xe9: //APP9
		case 0xea: //APP10
		case 0xeb: //APP11
		case 0xec: //APP12
		case 0xed: //APP13
		case 0xee: //APP14
		case 0xef: //APP15
		case 0xdb: //dqt
		case 0xc4: //dht
			ofst += j + 4;
			break;
		case 0xc0:
		case 0xc1:
		case 0xc2:
		case 0xc3:
			fd->GetRealData(ofst + 4, 14, BYTEARR(buff));
			height.Set(ReadMUInt16(&buff[1]));
			width.Set(ReadMUInt16(&buff[3]));
			ofst += j + 4;
			return true;
		default:
			return false;
		}
	}
	return true;
}

void Media::JPEGFile::WriteJPGBuffer(NotNullPtr<IO::Stream> stm, const UInt8 *jpgBuff, UOSInt buffSize, Media::RasterImage *oriImg)
{
	if (oriImg != 0 && (!oriImg->exif.IsNull() || oriImg->info.color.GetRAWICC() != 0) && jpgBuff[0] == 0xff && jpgBuff[1] == 0xd8)
	{
		UOSInt i;
		UOSInt j;
		i = 2;
		stm->Write(jpgBuff, 2);
		while (true)
		{
			if (i >= buffSize)
			{
				break;
			}
			if (jpgBuff[i] != 0xff)
			{
				stm->Write(&jpgBuff[i], buffSize - i);
				break;
			}
			if (jpgBuff[i + 1] == 0xdb)
			{
				const UInt8 *iccBuff = oriImg->info.color.GetRAWICC();
				if (iccBuff)
				{
					UOSInt iccLeng = ReadMUInt32(iccBuff);
					UInt8 iccHdr[18];
					iccHdr[0] = 0xff;
					iccHdr[1] = 0xe2;
					WriteMInt16(&iccHdr[2], iccLeng + 16);
					Text::StrConcatC(&iccHdr[4], UTF8STRC("ICC_PROFILE"));
					iccHdr[16] = 1;
					iccHdr[17] = 1;
					stm->Write(iccHdr, 18);
					stm->Write(iccBuff, iccLeng);
				}
				NotNullPtr<Media::EXIFData> exif;
				if (oriImg->exif.SetTo(exif))
				{
					/////////////////////////////////////
					exif = exif->Clone();
					exif->Remove(254); //NewSubfileType
					exif->Remove(256); //Width
					exif->Remove(257); //Height
					exif->Remove(258); //BitPerSample
					exif->Remove(259); //Compression
					exif->Remove(262); //PhotometricInterpretation
					exif->Remove(273); //StripOffsets
					exif->Remove(277); //SamplePerPixel
					exif->Remove(278); //RowsPerStrip
					exif->Remove(279); //StripByteCounts
					exif->Remove(284); //PlanarConfiguration
					exif->Remove(700); //Photoshop XMP
					exif->Remove(33723); //IPTC/NAA
					exif->Remove(34377); //PhotoshopImageResources
					exif->Remove(34675); //ICC Profile
					UInt64 exifSize;
					UInt64 endOfst;
					UInt32 k;
					UInt32 l;

					UInt8 *exifBuff;
					exif->GetExifBuffSize(exifSize, endOfst);
					while (exifSize + 16 >= 65536)
					{
						printf("Image EXIF oversized (%lld), removing largest\r\n", exifSize);
						if (!exif->RemoveLargest())
							break;
						exif->GetExifBuffSize(exifSize, endOfst);
					}
					exifBuff = MemAlloc(UInt8, exifSize + 18);
					exifBuff[0] = 0xff;
					exifBuff[1] = 0xe1;
					WriteMInt16(&exifBuff[2], exifSize + 16);
					WriteInt32(&exifBuff[4], ReadInt32((const UInt8*)"Exif"));
					WriteInt16(&exifBuff[8], 0);
					WriteInt16(&exifBuff[10], ReadInt16((const UInt8*)"II"));
					WriteInt16(&exifBuff[12], 42);
					WriteInt32(&exifBuff[14], 8);
					k = 8;
					l = (UInt32)(endOfst + 8);
					exif->ToExifBuff(&exifBuff[10], k, l);
					stm->Write(exifBuff, exifSize + 18);
					MemFree(exifBuff);
					exif.Delete();
				}

				stm->Write(&jpgBuff[i], buffSize - i);
				break;
			}
			else if (jpgBuff[i + 1] == 0xe1)
			{
				i += (UOSInt)ReadMUInt16(&jpgBuff[i + 2]) + 2;
			}
			else
			{
				j = (UOSInt)ReadMUInt16(&jpgBuff[i + 2]) + 2;
				stm->Write(&jpgBuff[i], j);
				i += j;
			}
		}
	}
	else
	{
		stm->Write(jpgBuff, buffSize);
	}
}
