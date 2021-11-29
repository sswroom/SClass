#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Exporter/GUIPNGExporter.h"
#include "IO/MemoryStream.h"
#include "Media/ImageList.h"
#include "Text/MyString.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

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
		Text::StrConcat(nameBuff, (const UTF8Char*)"PNG file (GDK)");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.png");
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	UInt8 *tmpBuff;
	GdkPixbuf *image = (GdkPixbuf*)ToImage(pobj, &tmpBuff);
	if (image == 0)
	{
		return false;
	}

	gchar *buff = 0;
	gsize buffSize;

	gdk_pixbuf_save_to_buffer(image, &buff, &buffSize, "png", 0, (void*)0);
	g_object_unref(image);
	if (tmpBuff)
	{
		MemFreeA(tmpBuff);
	}

	if (buff)
	{
		Media::Image *srcImg = 0;
		Media::ImageList *imgList;
		UInt8 *pngBuff;
		UOSInt pngSize;
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			imgList = (Media::ImageList*)pobj;
			srcImg = imgList->GetImage(0, 0);
		}
		pngBuff = (UInt8*)buff;
		pngSize = buffSize;
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
						Text::StrConcat((Char*)&chunkBuff[8], "Photoshop ICC profile");
						chunkBuff[30] = 0;

						UOSInt compSize = Data::Compress::Inflate::Compress(iccBuff, iccSize, &chunkBuff[31], true);
						if (compSize > 0)
						{
							UInt8 crcVal[4];
							Crypto::Hash::CRC32R crc;
							WriteMUInt32(chunkBuff, 23 + compSize);
							crc.Calc(&chunkBuff[4], 27 + compSize);
							crc.GetValue((UInt8*)&crcVal);
							WriteMUInt32(&chunkBuff[31 + compSize], ReadMUInt32(crcVal));
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
		g_free(buff);
		return true;
	}
	return false;
}
