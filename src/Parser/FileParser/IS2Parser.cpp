#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/IS2Parser.h"
#include "Text/MyStringW.h"

Parser::FileParser::IS2Parser::IS2Parser()
{
}

Parser::FileParser::IS2Parser::~IS2Parser()
{
}

Int32 Parser::FileParser::IS2Parser::GetName()
{
	return *(Int32*)"IS2P";
}

void Parser::FileParser::IS2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.is2", (const UTF8Char*)"IS2 Infra-red Image File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::IS2Parser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::IS2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	UInt8 *currBuff;
	if (fd->GetRealData(0, 204, buff) != 204)
		return 0;

	if (buff[0] != 1 || buff[1] != 2 || buff[2] != 2 || buff[3] != 0)
		return 0;

	if (ReadInt32(&buff[4]) != 4 || ReadInt32(&buff[8]) != 20)
		return 0;

	UInt32 totalSize = 204;
	UInt32 currSize;
	OSInt i = 44;
	OSInt j;
	while (i < 204)
	{
		currSize = ReadInt32(&buff[i + 4]);
		if (currSize == 0)
			break;
		totalSize += currSize;
		i += 8;
	}
	if (totalSize != fd->GetDataSize())
		return 0;
	Bool valid = true;
	OSInt currOfst = 204;
	Int32 currType;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
	i = 44;
	while (i < 204 && valid)
	{
		currType = ReadInt32(&buff[i]);
		currSize = ReadInt32(&buff[i + 4]);
		if (currSize == 0)
			break;
		currBuff = MemAlloc(UInt8, currSize);
		if (currSize != fd->GetRealData(currOfst, currSize, currBuff))
		{
			valid = false;
		}
		else
		{
			if (ReadUInt32(&currBuff[4]) != currSize)
			{
				valid = false;
			}
			else
			{
				switch (currType)
				{
				case 0x3: //Camera Info
					{
						Data::DateTime dt;
						Int32 irWidth = ReadInt32(&currBuff[20]);
						Int32 irHeight = ReadInt32(&currBuff[24]);
						Int32 visibleWidth = ReadInt32(&currBuff[28]);
						Int32 visibleHeight = ReadInt32(&currBuff[32]);
						Int32 firmwareYear = ReadInt16(&currBuff[50]);
						Int32 firmwareMonth = currBuff[49];
						Int32 firmwareDay = currBuff[48];
//						Int32 firmwareMajor = currBuff[71];
//						Int32 firmwareMinor = currBuff[70];
//						Int32 firmwareBuild = ReadInt16(&currBuff[68]);
						const UTF16Char *cameraBrand = (const UTF16Char *)&currBuff[72];
						const UTF16Char *cameraModel = (const UTF16Char *)&currBuff[136];
						const UTF16Char *cameraSN = (const UTF16Char *)&currBuff[200];
						const UTF8Char *u8ptr = Text::StrToUTF8New(cameraBrand);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_BRAND, u8ptr);
						Text::StrDelNew(u8ptr);
						u8ptr = Text::StrToUTF8New(cameraModel);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_MODEL, u8ptr);
						Text::StrDelNew(u8ptr);
						u8ptr = Text::StrToUTF8New(cameraSN);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_SN, u8ptr);
						Text::StrDelNew(u8ptr);
						imgList->SetValueInt32(Media::ImageList::VT_IR_WIDTH, irWidth);
						imgList->SetValueInt32(Media::ImageList::VT_IR_HEIGHT, irHeight);
						imgList->SetValueInt32(Media::ImageList::VT_VISIBLE_WIDTH, visibleWidth);
						imgList->SetValueInt32(Media::ImageList::VT_VISIBLE_HEIGHT, visibleHeight);
						dt.SetValue(firmwareYear, firmwareMonth, firmwareDay, 0, 0, 0, 0, 0);
						imgList->SetValueInt64(Media::ImageList::VT_FIRMWARE_DATE, dt.ToTicks());
						imgList->SetValueInt32(Media::ImageList::VT_FIRMWARE_VERSION, ReadInt32(&currBuff[68]));
					}
					break;
				case 0x42: //Capture Info
					{
						Int32 captureWidth;
						Int32 captureHeight;
						Data::DateTime captureTime;
						captureWidth = ReadInt32(&currBuff[8]);
						captureHeight = ReadInt32(&currBuff[12]);
						captureTime.ToLocalTime();
						captureTime.SetValue(ReadInt16(&currBuff[22]), currBuff[21], currBuff[20], 0, 0, 0, 0);
						captureTime.AddMS(ReadInt32(&currBuff[16]));
						imgList->SetValueInt32(Media::ImageList::VT_CAPTURE_WIDTH, captureWidth);
						imgList->SetValueInt32(Media::ImageList::VT_CAPTURE_HEIGHT, captureHeight);
						imgList->SetValueInt64(Media::ImageList::VT_CAPTURE_DATE, captureTime.ToTicks());
					}
					break;
				case 0x18: //Visible Image
					{
						Int32 imageWidth = ReadInt32(&currBuff[12]);
						Int32 imageHeight = ReadInt32(&currBuff[16]);
						Media::StaticImage *img;
						NEW_CLASS(img, Media::StaticImage(imageWidth, imageHeight, 0, 32, Media::PF_B8G8R8A8, imageWidth * imageHeight * 4, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UInt8 *dataPtr = img->data;
						UInt8 *currBuffPtr = currBuff + 60;
						Int32 cnt = imageWidth * imageHeight;
						while (cnt-- > 0)
						{
							UInt32 v = *(UInt16*)currBuffPtr;
							UInt8 r;
							UInt8 g;
							UInt8 b;
							r = v >> 11;
							b = v & 0x1f;
							g = (v >> 5) & 0x3f;
							dataPtr[0] = (b << 3) | (b >> 2);
							dataPtr[1] = (g << 2) | (g >> 4);
							dataPtr[2] = (r << 3) | (r >> 2);
							dataPtr[3] = 0xff;
							currBuffPtr += 2;
							dataPtr += 4;
						}
						j = imgList->AddImage(img, 0);
						imgList->SetImageType(j, Media::ImageList::IT_VISIBLEIMAGE);
					}
					break;
				case 0x19: //IR Info
					{
//						Double v1 = ReadFloat(&currBuff[12]);
//						Double v2 = ReadFloat(&currBuff[16]);
//						Double v3 = ReadFloat(&currBuff[36]);
//						Int32 unk = ReadInt32(&currBuff[44]);

					}
					break;
				case 0x11: //Thermo Image
					{
						// [0] = 4
						// [4] = size
						// [8] = 0
						Int32 imageWidth = ReadInt32(&currBuff[12]);
						Int32 imageHeight = ReadInt32(&currBuff[16]);
						// [20] = 1
						// [24] = 0
						// [28] = 32
						// [32] = 0
						Double emissivity = ReadFloat(&currBuff[36]); //emissivity 
						Double transmission = ReadFloat(&currBuff[40]); //transmission
						Double bkgTemp = ReadFloat(&currBuff[44]);
						// [48] = 0
						// [52] = 0
						// [56] = 0
						// t = tb / (emissivity * transmission) - (2 - emissivity - transmission) * backgroundtemp;
						Int32 minVal = ReadInt16(&currBuff[60]); //?
						Int32 maxVal = ReadInt16(&currBuff[62]); //?
						Int32 valDiff = maxVal - minVal;
						Media::StaticImage *img;
						NEW_CLASS(img, Media::StaticImage(imageWidth, imageHeight, 0, 16, Media::PF_LE_W16, imageWidth * imageHeight * 2, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UInt8 *dataPtr = img->data;
						UInt8 *currBuffPtr = currBuff + 64;
						Int32 cnt = imageWidth * imageHeight;
						Int32 v;
						while (cnt-- > 0)
						{
							v = ((*(Int16*)currBuffPtr) - minVal) * 65536 / valDiff;
							if (v < 0)
								v = 0;
							else if (v > 65535)
								v = 65535;
							WriteInt16(dataPtr, v);
							currBuffPtr += 2;
							dataPtr += 2;
						}
						j = imgList->AddImage(img, 0);
						imgList->SetImageType(j, Media::ImageList::IT_IRIMAGE);
						imgList->SetThermoImage(imageWidth, imageHeight, 16, currBuff + 64, emissivity, transmission, bkgTemp, Media::ImageList::TT_UNKNOWN);
					}
					break;
				case 0x22: //Output Image
					{
						Int32 imageWidth = ReadInt32(&currBuff[20]);
						Int32 imageHeight = ReadInt32(&currBuff[24]);
						Media::StaticImage *img;
						NEW_CLASS(img, Media::StaticImage(imageWidth, imageHeight, 0, 32, Media::PF_B8G8R8A8, imageWidth * imageHeight * 4, 0, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UInt8 *dataPtr = img->data;
						UInt8 *currBuffPtr = currBuff + 56;
						Int32 cnt = imageWidth * imageHeight;
						while (cnt-- > 0)
						{
							UInt32 v = *(UInt16*)currBuffPtr;
							UInt8 r;
							UInt8 g;
							UInt8 b;
							r = v >> 11;
							b = v & 0x1f;
							g = (v >> 5) & 0x3f;
							dataPtr[0] = (b << 3) | (b >> 2);
							dataPtr[1] = (g << 2) | (g >> 4);
							dataPtr[2] = (r << 3) | (r >> 2);
							dataPtr[3] = 0xff;
							currBuffPtr += 2;
							dataPtr += 4;
						}
						j = imgList->AddImage(img, 0);
						imgList->SetImageType(j, Media::ImageList::IT_OUTPUTIMAGE);
					}
					break;
				}
			}
		}
		MemFree(currBuff);
		currOfst += currSize;
		i += 8;
	}
	if (!valid)
	{
		DEL_CLASS(imgList);
		return 0;
	}

	return imgList;
}
