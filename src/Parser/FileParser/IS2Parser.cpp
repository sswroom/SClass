#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
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

void Parser::FileParser::IS2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.is2"), CSTR("IS2 Infra-red Image File"));
	}
}

IO::ParserType Parser::FileParser::IS2Parser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::IS2Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (fd->GetDataSize() < 204)
		return 0;

	if (hdr[0] != 1 || hdr[1] != 2 || hdr[2] != 2 || hdr[3] != 0)
		return 0;

	if (ReadInt32(&hdr[4]) != 4 || ReadInt32(&hdr[8]) != 20)
		return 0;

	UInt32 totalSize = 204;
	UInt32 currSize;
	UOSInt i = 44;
	UOSInt j;
	while (i < 204)
	{
		currSize = ReadUInt32(&hdr[i + 4]);
		if (currSize == 0)
			break;
		totalSize += currSize;
		i += 8;
	}
	if (totalSize != fd->GetDataSize())
		return 0;
	Bool valid = true;
	UOSInt currOfst = 204;
	Int32 currType;
	Media::ImageList *imgList;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));
	i = 44;
	while (i < 204 && valid)
	{
		currType = ReadInt32(&hdr[i]);
		currSize = ReadUInt32(&hdr[i + 4]);
		if (currSize == 0)
			break;
		Data::ByteBuffer currBuff(currSize);
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
						UInt16 firmwareYear = ReadUInt16(&currBuff[50]);
						Int32 firmwareMonth = currBuff[49];
						Int32 firmwareDay = currBuff[48];
//						Int32 firmwareMajor = currBuff[71];
//						Int32 firmwareMinor = currBuff[70];
//						Int32 firmwareBuild = ReadInt16(&currBuff[68]);
						const UTF16Char *cameraBrand = (const UTF16Char *)&currBuff[72];
						const UTF16Char *cameraModel = (const UTF16Char *)&currBuff[136];
						const UTF16Char *cameraSN = (const UTF16Char *)&currBuff[200];
						NN<Text::String> s = Text::String::NewNotNull(cameraBrand);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_BRAND, s->ToCString());
						s->Release();
						s = Text::String::NewNotNull(cameraModel);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_MODEL, s->ToCString());
						s->Release();
						s = Text::String::NewNotNull(cameraSN);
						imgList->SetValueStr(Media::ImageList::VT_CAMERA_SN, s->ToCString());
						s->Release();
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
						captureTime.SetValue(ReadUInt16(&currBuff[22]), currBuff[21], currBuff[20], 0, 0, 0, 0);
						captureTime.AddMS(ReadInt32(&currBuff[16]));
						imgList->SetValueInt32(Media::ImageList::VT_CAPTURE_WIDTH, captureWidth);
						imgList->SetValueInt32(Media::ImageList::VT_CAPTURE_HEIGHT, captureHeight);
						imgList->SetValueInt64(Media::ImageList::VT_CAPTURE_DATE, captureTime.ToTicks());
					}
					break;
				case 0x18: //Visible Image
					{
						UInt32 imageWidth = ReadUInt32(&currBuff[12]);
						UInt32 imageHeight = ReadUInt32(&currBuff[16]);
						NN<Media::StaticImage> img;
						NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(imageWidth, imageHeight), 0, 32, Media::PF_B8G8R8A8, imageWidth * imageHeight * 4, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UnsafeArray<UInt8> dataPtr = img->data;
						Data::ByteArray currBuffPtr = currBuff + 60;
						UInt32 cnt = imageWidth * imageHeight;
						while (cnt-- > 0)
						{
							UInt32 v = currBuffPtr.ReadNU16(0);
							UInt8 r;
							UInt8 g;
							UInt8 b;
							r = (UInt8)(v >> 11);
							b = (UInt8)(v & 0x1f);
							g = (UInt8)((v >> 5) & 0x3f);
							dataPtr[0] = (UInt8)((b << 3) | (b >> 2));
							dataPtr[1] = (UInt8)((g << 2) | (g >> 4));
							dataPtr[2] = (UInt8)((r << 3) | (r >> 2));
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
						UInt32 imageWidth = ReadUInt32(&currBuff[12]);
						UInt32 imageHeight = ReadUInt32(&currBuff[16]);
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
						NN<Media::StaticImage> img;
						NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(imageWidth, imageHeight), 0, 16, Media::PF_LE_W16, imageWidth * imageHeight * 2, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UnsafeArray<UInt8> dataPtr = img->data;
						Data::ByteArray currBuffPtr = currBuff + 64;
						UInt32 cnt = imageWidth * imageHeight;
						Int32 v;
						while (cnt-- > 0)
						{
							v = (currBuffPtr.ReadNI16(0) - minVal) * 65536 / valDiff;
							if (v < 0)
								v = 0;
							else if (v > 65535)
								v = 65535;
							WriteInt16(&dataPtr[0], v);
							currBuffPtr += 2;
							dataPtr += 2;
						}
						j = imgList->AddImage(img, 0);
						imgList->SetImageType(j, Media::ImageList::IT_IRIMAGE);
						imgList->SetThermoImage(Math::Size2D<UOSInt>(imageWidth, imageHeight), 16, currBuff.Arr() + 64, emissivity, transmission, bkgTemp, Media::ImageList::TT_UNKNOWN);
					}
					break;
				case 0x22: //Output Image
					{
						UInt32 imageWidth = ReadUInt32(&currBuff[20]);
						UInt32 imageHeight = ReadUInt32(&currBuff[24]);
						NN<Media::StaticImage> img;
						NEW_CLASSNN(img, Media::StaticImage(Math::Size2D<UOSInt>(imageWidth, imageHeight), 0, 32, Media::PF_B8G8R8A8, imageWidth * imageHeight * 4, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_TOP_CENTER));
						UnsafeArray<UInt8> dataPtr = img->data;
						Data::ByteArray currBuffPtr = currBuff + 56;
						UInt32 cnt = imageWidth * imageHeight;
						while (cnt-- > 0)
						{
							UInt32 v = currBuffPtr.ReadNU16(0);
							UInt8 r;
							UInt8 g;
							UInt8 b;
							r = (UInt8)(v >> 11);
							b = (UInt8)(v & 0x1f);
							g = (UInt8)((v >> 5) & 0x3f);
							dataPtr[0] = (UInt8)((b << 3) | (b >> 2));
							dataPtr[1] = (UInt8)((g << 2) | (g >> 4));
							dataPtr[2] = (UInt8)((r << 3) | (r >> 2));
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
