#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Exporter/GUIPNGExporter.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"
#include "Text/MyString.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include "Win32/COMStream.h"
#ifndef _WIN32_WCE
#include <gdiplus.h>
#endif

Exporter::GUIPNGExporter::GUIPNGExporter() : Exporter::GUIExporter()
{
}

Exporter::GUIPNGExporter::~GUIPNGExporter()
{
}

Int32 Exporter::GUIPNGExporter::GetName()
{
	return *(Int32*)"GPPN";
}

Bool Exporter::GUIPNGExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("PNG file (GDI+)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.png"));
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
#ifdef _WIN32_WCE
	return false;
#else
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;
	UInt8 *relBuff;

	image = (Gdiplus::Image*)ToImage(pobj, &relBuff);
	if (image == 0)
	{
		return false;
	}

	if (GetEncoderClsid(L"image/png", &encoderClsid) < 0)
	{
		DEL_CLASS(image);
		if (relBuff)
		{
			MemFreeA(relBuff);
		}
		return false;
	}

	IO::MemoryStream mstm;
	{
		Win32::COMStream cstm(mstm);
		NN<ParamData> para;

		if (param.SetTo(para))
		{
			Gdiplus::EncoderParameters params;
			params.Count = 1;
			params.Parameter[0].Guid = Gdiplus::EncoderQuality;
			params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
			params.Parameter[0].NumberOfValues = 1;
			params.Parameter[0].Value = para.Ptr();
			stat = image->Save(&cstm, &encoderClsid, &params);
		}
		else
		{
			stat = image->Save(&cstm, &encoderClsid, 0);
		}
	}
	DEL_CLASS(image);
	if (relBuff)
	{
		MemFreeA(relBuff);
	}

	if (stat != Gdiplus::Ok)
	{
		return false;
	}

	Media::RasterImage *srcImg = 0;
	NN<Media::ImageList> imgList;
	UInt8 *pngBuff;
	UOSInt pngSize;
	if (pobj->GetParserType() == IO::ParserType::ImageList)
	{
		imgList = NN<Media::ImageList>::ConvertFrom(pobj);
		srcImg = imgList->GetImage(0, 0);
	}
	pngBuff = mstm.GetBuff(pngSize);
	if (srcImg != 0 && pngBuff[0] == 0x89 && pngBuff[1] == 0x50 && pngBuff[2] == 0x4e && pngBuff[3] == 0x47)
	{
		const UInt8 *iccBuff = srcImg->info.color.GetRAWICC();
		if (iccBuff)
		{
			UInt32 chunkSize;
			Int32 chunkType;
			UOSInt i;
			stm->Write(pngBuff, 8);
			i = 8;
			while (true)
			{
				if (i >= pngSize)
				{
					break;
				}
				chunkSize = ReadMUInt32(&pngBuff[i]);
				chunkType = ReadInt32(&pngBuff[i + 4]);
				if (chunkType == ReadInt32("sRGB"))
				{
					i += chunkSize + 12;
				}
				else if (chunkType == ReadInt32("gAMA"))
				{
					i += chunkSize + 12;
				}
				else if (chunkType == ReadInt32("IDAT"))
				{
					UInt32 iccSize = ReadMUInt32(iccBuff);
					UInt8 *chunkBuff = MemAlloc(UInt8, iccSize + 32);
					WriteInt32(&chunkBuff[4], ReadInt32("iCCP"));
					Text::StrConcatC((UTF8Char*)&chunkBuff[8], UTF8STRC("Photoshop ICC profile"));
					chunkBuff[30] = 0;
					UOSInt compSize = Data::Compress::Inflate::Compress(iccBuff, iccSize, &chunkBuff[31], true, Data::Compress::Inflate::CompressionLevel::BestCompression);
					if (compSize > 0)
					{
						UInt8 crcBuff[4];
						Crypto::Hash::CRC32R crc;
						WriteMUInt32(chunkBuff, 23 + compSize);
						crc.Calc(&chunkBuff[4], 27 + compSize);
						crc.GetValue(crcBuff);
						WriteMUInt32(&chunkBuff[31 + compSize], ReadMUInt32(crcBuff));
						stm->Write(chunkBuff, 35 + compSize);
					}
					MemFree(chunkBuff);

					stm->Write(&pngBuff[i], pngSize - i);
					break;
				}
				else
				{
					stm->Write(&pngBuff[i], chunkSize + 12);
					i += chunkSize + 12;
				}
			}
		}
		else
		{
			stm->Write(pngBuff, pngSize);
		}
	}
	else
	{
		stm->Write(pngBuff, pngSize);
	}
	return true;
#endif

}
