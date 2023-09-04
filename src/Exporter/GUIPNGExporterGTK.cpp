#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "Data/Compress/Inflate.h"
#include "Exporter/GUIPNGExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Unit/Distance.h"
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
		Text::StrConcatC(nameBuff, UTF8STRC("PNG file (GDK)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.png"));
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
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
			UInt8 tmpBuff[64];
			const UInt8 *iccBuff = srcImg->info.color.GetRAWICC();
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
				else if (chunkType == ReadInt32("cHRM"))
				{
					i += chunkSize + 12;
				}
				else if (chunkType == ReadInt32("pHYs"))
				{
					i += chunkSize + 12;
				}
				else if (chunkType == ReadInt32("IDAT"))
				{
					Crypto::Hash::CRC32R crc;
					if (iccBuff)
					{
						UInt32 iccSize = ReadMUInt32(iccBuff);
						UInt8 *chunkBuff = MemAlloc(UInt8, iccSize + 32);
						WriteInt32(&chunkBuff[4], ReadInt32("iCCP"));
						Text::StrConcatC((UTF8Char*)&chunkBuff[8], UTF8STRC("Photoshop ICC profile"));
						chunkBuff[30] = 0;

						UOSInt compSize = Data::Compress::Inflate::Compress(iccBuff, iccSize, &chunkBuff[31], true, Data::Compress::Inflate::CompressionLevel::BestCompression);
						if (compSize > 0)
						{
							WriteMUInt32(chunkBuff, 23 + compSize);
							WriteMUInt32(&chunkBuff[31 + compSize], crc.CalcDirect(&chunkBuff[4], 27 + compSize));
							stm->Write(chunkBuff, 35 + compSize);
						}
						MemFree(chunkBuff);
					}
					else
					{
						if (srcImg->info.color.GetRTranParamRead()->GetTranType() == Media::CS::TRANT_sRGB)
						{
							WriteMUInt32(&tmpBuff[0], 1);
							WriteInt32(&tmpBuff[4], ReadInt32("sRGB"));
							tmpBuff[8] = 0;
							WriteMUInt32(&tmpBuff[9], crc.CalcDirect(&tmpBuff[4], 5));
							stm->Write(tmpBuff, 13);
						}

						NotNullPtr<const Media::ColorProfile::ColorPrimaries> prim = srcImg->info.color.GetPrimariesRead();
						WriteMUInt32(&tmpBuff[0], 32);
						WriteInt32(&tmpBuff[4], ReadInt32("cHRM"));
						WriteMInt32(&tmpBuff[8], Double2Int32(prim->wx * 100000));
						WriteMInt32(&tmpBuff[12], Double2Int32(prim->wy * 100000));
						WriteMInt32(&tmpBuff[16], Double2Int32(prim->rx * 100000));
						WriteMInt32(&tmpBuff[20], Double2Int32(prim->ry * 100000));
						WriteMInt32(&tmpBuff[24], Double2Int32(prim->gx * 100000));
						WriteMInt32(&tmpBuff[28], Double2Int32(prim->gy * 100000));
						WriteMInt32(&tmpBuff[32], Double2Int32(prim->bx * 100000));
						WriteMInt32(&tmpBuff[36], Double2Int32(prim->by * 100000));
						WriteMUInt32(&tmpBuff[40], crc.CalcDirect(&tmpBuff[4], 36));
						stm->Write(tmpBuff, 44);
					}
					if (srcImg->info.hdpi != 72.0 || srcImg->info.vdpi != 72.0)
					{
						Int32 hVal = Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / srcImg->info.hdpi));
						Int32 vVal = Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / srcImg->info.vdpi));
						WriteMInt32(&tmpBuff[0], 9);
						*(Int32*)&tmpBuff[4] = *(Int32*)"pHYs";
						WriteMInt32(&tmpBuff[8], hVal);
						WriteMInt32(&tmpBuff[12], vVal);
						tmpBuff[16] = 1;
						WriteMUInt32(&tmpBuff[17], crc.CalcDirect(&tmpBuff[4], 13));
						stm->Write(tmpBuff, 21);
					}

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
		g_free(buff);
		return true;
	}
	return false;
}
