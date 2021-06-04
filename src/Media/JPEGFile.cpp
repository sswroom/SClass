#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryData.h"
#include "Media/JPEGFile.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

//http://stackoverflow.com/questions/662565/how-to-create-huffman-tree-from-ffc4-dht-header-in-jpeg-file
//http://u88.n24.queensu.ca/exiftool/forum/index.php?topic=4898.0 FLIR

Bool Media::JPEGFile::ParseJPEGHeader(IO::IStreamData *fd, Media::Image *img, Media::ImageList *imgList, Parser::ParserList *parsers)
{
	UInt64 ofst;
	UInt32 nextOfst;
	UInt32 j;
	UInt8 buff[18];
	UInt8 *tagBuff;
	Bool ret = false;
	IO::MemoryStream *flirMstm = 0;
	UInt8 flirMaxSegm;
	UInt8 flirCurrSegm = 0;

	if (fd->GetRealData(0, 2, buff) != 2)
		return false;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return false;
	ofst = 2;
	while (true)
	{
		if (fd->GetRealData(ofst, 4, buff) != 4)
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
			fd->GetRealData(ofst + 4, 14, buff);
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				Media::EXIFData::RInt32Func readInt32;
				Media::EXIFData::RInt16Func readInt16;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					readInt32 = Media::EXIFData::TReadInt32;
					readInt16 = Media::EXIFData::TReadInt16;
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					readInt32 = Media::EXIFData::TReadMInt32;
					readInt16 = Media::EXIFData::TReadMInt16;
				}
				else
				{
					ret = false;
					break;
				}
				if (readInt16(&buff[8]) != 42)
				{
					ret = false;
					break;
				}
				if (readInt32(&buff[10]) != 8)
				{
					ret = false;
					break;
				}
				if (img->exif)
				{
					DEL_CLASS(img->exif);
				}
				img->exif = Media::EXIFData::ParseIFD(fd, ofst + 18, readInt32, readInt16, &nextOfst, ofst + 10);
				ofst += j + 4;
			}
			else if (*(Int32*)buff == *(Int32*)"FLIR")
			{
				if (buff[4] == 0 && buff[5] == 1)
				{
					if (flirMstm == 0 && buff[6] == 0)
					{
						flirMaxSegm = buff[7];
						NEW_CLASS(flirMstm, IO::MemoryStream((const UTF8Char*)"Media.JPEGFile.ParseJPEGHeader.flirMstm"));
						flirCurrSegm = buff[6];
						tagBuff = MemAlloc(UInt8, j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
						MemFree(tagBuff);
					}
					else if (flirMstm && buff[6] == (flirCurrSegm + 1))
					{
						flirCurrSegm = (UInt8)(flirCurrSegm + 1);
						tagBuff = MemAlloc(UInt8, j);
						fd->GetRealData(ofst + 4, j, tagBuff);
						flirMstm->Write(&tagBuff[8], j - 8);
						MemFree(tagBuff);
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
			tagBuff = MemAlloc(UInt8, j);
			fd->GetRealData(ofst + 4, j, tagBuff);
			if (Text::StrCompare((Char*)tagBuff, "ICC_PROFILE") == 0)
			{
				Media::ICCProfile *icc = Media::ICCProfile::Parse(&tagBuff[14], j - 14);
				if (icc)
				{
					icc->GetRedTransferParam(img->info->color->GetRTranParam());
					icc->GetGreenTransferParam(img->info->color->GetGTranParam());
					icc->GetBlueTransferParam(img->info->color->GetBTranParam());
					icc->GetColorPrimaries(img->info->color->GetPrimaries());
					img->info->color->SetRAWICC(&tagBuff[14]);
					DEL_CLASS(icc);
				}
			}
			MemFree(tagBuff);
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
			UOSInt buffSize;
			UInt32 blkOfst;
			UInt32 blkSize;
//			UInt32 blkType;
			UInt32 delay;
			tagBuff = flirMstm->GetBuff(&buffSize);
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
						IO::StmData::MemoryData *mfd;
						Media::ImageList *innerImgList;
						Media::Image *innerImg;
						NEW_CLASS(mfd, IO::StmData::MemoryData(&tagBuff[blkOfst + 32], blkSize - 32));
						innerImgList = (Media::ImageList*)parsers->ParseFileType(mfd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
						DEL_CLASS(mfd);
						if (innerImgList)
						{
							k = innerImgList->GetCount();
							i = 0;
							while (i < k)
							{
								innerImg = innerImgList->GetImage(i, &delay);
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
							imgList->SetThermoImage(innerW, innerH, 16, &tagBuff[blkOfst + 32], 0.95, 0, 0, Media::ImageList::TT_FLIR);
						}
						else if (tagBuff[blkOfst + 32] == 0x89 && tagBuff[blkOfst + 33] == 0x50 && tagBuff[blkOfst + 34] == 0x4e && tagBuff[blkOfst + 35] == 0x47)
						{
							IO::StmData::MemoryData *mfd;
							Media::ImageList *innerImgList;
							Media::Image *innerImg;
							Media::StaticImage *stImg;
							NEW_CLASS(mfd, IO::StmData::MemoryData(&tagBuff[blkOfst + 32], blkSize - 32));
							innerImgList = (Media::ImageList*)parsers->ParseFileType(mfd, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
							DEL_CLASS(mfd);
							if (innerImgList)
							{
								k = innerImgList->GetCount();
								i = 0;
								while (i < k)
								{
									innerImg = innerImgList->GetImage(i, &delay);
									stImg = innerImg->CreateStaticImage();
									if (stImg->info->pf == Media::PF_LE_W16)
									{
										UInt8 *imgPtr = stImg->data;
										UOSInt pixelCnt = stImg->info->dispWidth * stImg->info->dispHeight;
										while (pixelCnt-- > 0)
										{
											WriteInt16(imgPtr, ReadMInt16(imgPtr));
											imgPtr += 2;
										}
										imgList->SetThermoImage(stImg->info->dispWidth, stImg->info->dispHeight, 16, stImg->data, 0.95, 0, 0, Media::ImageList::TT_FLIR);
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

Media::EXIFData *Media::JPEGFile::ParseJPEGExif(IO::IStreamData *fd)
{
	UInt64 ofst;
	UInt32 nextOfst;
	UInt32 j;
	UInt8 buff[18];
	if (fd->GetRealData(0, 2, buff) != 2)
		return 0;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return 0;
	ofst = 2;
	while (true)
	{
		if (fd->GetRealData(ofst, 4, buff) != 4)
			return 0;
		if (buff[0] != 0xff)
			return 0;
		if (buff[1] == 0xdb)
			return 0;

		j = (UInt32)((buff[2] << 8) | buff[3]) - 2;
		if (buff[1] == 0xe1)
		{
			fd->GetRealData(ofst + 4, 14, buff);
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				Media::EXIFData::RInt32Func readInt32;
				Media::EXIFData::RInt16Func readInt16;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					readInt32 = Media::EXIFData::TReadInt32;
					readInt16 = Media::EXIFData::TReadInt16;
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					readInt32 = Media::EXIFData::TReadMInt32;
					readInt16 = Media::EXIFData::TReadMInt16;
				}
				else
				{
					return 0;
				}
				if (readInt16(&buff[8]) != 42)
					return 0;
				if (readInt32(&buff[10]) != 8)
					return 0;
				Media::EXIFData *exif = Media::EXIFData::ParseIFD(fd, ofst + 18, readInt32, readInt16, &nextOfst, ofst + 10);
				if (exif)
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

Bool Media::JPEGFile::ParseJPEGHeaders(IO::IStreamData *fd, Media::EXIFData **exif, Text::XMLDocument **xmf, Media::ICCProfile **icc, Int32 *width, Int32 *height)
{
	UInt64 ofst;
	UInt32 nextOfst;
	UInt32 j;
	UInt8 buff[30];
	UInt8 *tagBuff;
	if (fd->GetRealData(0, 2, buff) != 2)
		return false;
	if (buff[0] != 0xff || buff[1] != 0xd8)
		return false;

	if (exif)
		*exif = 0;
	if (xmf)
		*xmf = 0;
	if (icc)
		*icc = 0;
	if (width)
		*width = 0;
	if (height)
		*height = 0;

	ofst = 2;
	while (ofst < fd->GetDataSize())
	{
		if (fd->GetRealData(ofst, 4, buff) != 4)
			return false;
		if (buff[0] != 0xff)
			return false;

		j = (UInt32)((buff[2] << 8) | buff[3]) - 2;
		switch (buff[1])
		{
		case 0xe1: //APP1
			fd->GetRealData(ofst + 4, 30, buff);
			if (*(Int32*)buff == *(Int32*)"Exif")
			{
				Media::EXIFData::RInt32Func readInt32;
				Media::EXIFData::RInt16Func readInt16;
				if (*(Int16*)&buff[6] == *(Int16*)"II")
				{
					readInt32 = Media::EXIFData::TReadInt32;
					readInt16 = Media::EXIFData::TReadInt16;
				}
				else if (*(Int16*)&buff[6] == *(Int16*)"MM")
				{
					readInt32 = Media::EXIFData::TReadMInt32;
					readInt16 = Media::EXIFData::TReadMInt16;
				}
				else
				{
					return false;
				}
				if (readInt16(&buff[8]) != 42)
					return false;
				if (readInt32(&buff[10]) != 8)
					return false;
				if (exif)
				{
					if (*exif)
					{
						DEL_CLASS(*exif);
						*exif = 0;
					}
					*exif = Media::EXIFData::ParseIFD(fd, ofst + 18, readInt32, readInt16, &nextOfst, ofst + 10);
				}	
				ofst += j + 4;
			}
			else if (buff[28] == 0 && Text::StrEquals((Char*)buff, "http://ns.adobe.com/xap/1.0/"))
			{
				if (xmf)
				{
					Text::EncodingFactory encFact;
					if (*xmf)
						DEL_CLASS(*xmf);
					tagBuff = MemAlloc(UInt8, j - 29);
					fd->GetRealData(ofst + 33, j - 29, tagBuff);
					NEW_CLASS(*xmf, Text::XMLDocument());
                    if ((*xmf)->ParseBuff(&encFact, tagBuff, j - 29))
					{
					}
					else
					{
						DEL_CLASS(*xmf);
						*xmf = 0;
					}
					MemFree(tagBuff);
				}
				ofst += j + 4;
			}
			else
			{
				ofst += j + 4;
			}
			break;
		case 0xe2: //APP2
			tagBuff = MemAlloc(UInt8, j);
			fd->GetRealData(ofst + 4, j, tagBuff);
			if (Text::StrCompare((Char*)tagBuff, "ICC_PROFILE") == 0)
			{
				if (icc)
				{
					Media::ICCProfile *newIcc = Media::ICCProfile::Parse(&tagBuff[14], j - 14);
					if (newIcc)
					{
						if (*icc)
						{
							DEL_CLASS(*icc);
						}
						*icc = newIcc;
					}
				}
			}
			MemFree(tagBuff);
			ofst += j + 4;
			break;
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
			fd->GetRealData(ofst + 4, 14, buff);
			if (height)
				*height = ReadMUInt16(&buff[1]);
			if (width)
				*width = ReadMInt16(&buff[3]);
			ofst += j + 4;
			return true;
		default:
			return false;
		}
	}
	return true;
}
