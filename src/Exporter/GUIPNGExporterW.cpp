#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
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
#include "miniz.h"

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
		Text::StrConcat(nameBuff, (const UTF8Char*)"PNG file (GDI+)");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.png");
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
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

	IO::MemoryStream *mstm;
	Win32::COMStream *cstm;
	NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Exporter.GUIPNGExporter.ExportFile.mstm"));
	NEW_CLASS(cstm, Win32::COMStream(mstm));

	if (param)
	{
		Gdiplus::EncoderParameters params;
		params.Count = 1;
		params.Parameter[0].Guid = Gdiplus::EncoderQuality;
		params.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
		params.Parameter[0].NumberOfValues = 1;
		params.Parameter[0].Value = param;
		stat = image->Save(cstm, &encoderClsid, &params);
	}
	else
	{
		stat = image->Save(cstm, &encoderClsid, 0);
	}

	DEL_CLASS(cstm);
	DEL_CLASS(image);
	if (relBuff)
	{
		MemFreeA(relBuff);
	}

	if (stat != Gdiplus::Ok)
	{
		DEL_CLASS(mstm);
		return false;
	}

	Media::Image *srcImg = 0;
	Media::ImageList *imgList;
	UInt8 *pngBuff;
	UOSInt pngSize;
	if (pobj->GetParserType() == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		imgList = (Media::ImageList*)pobj;
		srcImg = imgList->GetImage(0, 0);
	}
	pngBuff = mstm->GetBuff(&pngSize);
	if (srcImg != 0 && pngBuff[0] == 0x89 && pngBuff[1] == 0x50 && pngBuff[2] == 0x4e && pngBuff[3] == 0x47)
	{
		const UInt8 *iccBuff = srcImg->info->color->GetRAWICC();
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
				chunkSize = ReadMInt32(&pngBuff[i]);
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
					Int32 iccSize = ReadMInt32(iccBuff);
					UInt8 *chunkBuff = MemAlloc(UInt8, iccSize + 32);
					WriteInt32(&chunkBuff[4], ReadInt32("iCCP"));
					Text::StrConcat((Char*)&chunkBuff[8], "Photoshop ICC profile");
					chunkBuff[30] = 0;

					mz_stream dstm;
					int status;
					MemClear(&dstm, sizeof(mz_stream));
					dstm.next_in = iccBuff;
					dstm.avail_in = iccSize;
					dstm.next_out = &chunkBuff[31];
					dstm.avail_out = iccSize;
					mz_deflateInit2(&dstm, MZ_BEST_COMPRESSION, MZ_DEFLATED, MZ_DEFAULT_WINDOW_BITS, 1, 0);
					status = mz_deflate(&dstm, MZ_FINISH);
					mz_deflateEnd(&dstm);

					if (status == MZ_OK || status == MZ_STREAM_END)
					{
						UInt8 crcBuff[4];
						Crypto::Hash::CRC32R crc;
						WriteMInt32(chunkBuff, 23 + dstm.total_out);
						crc.Calc(&chunkBuff[4], 27 + dstm.total_out);
						crc.GetValue(crcBuff);
						WriteMInt32(&chunkBuff[31 + dstm.total_out], ReadMInt32(crcBuff));
						stm->Write(chunkBuff, 35 + dstm.total_out);
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
	DEL_CLASS(mstm);
	return true;
#endif

}
