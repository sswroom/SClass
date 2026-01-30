#include "Stdafx.h"
#include "Crypto/Hash/CRC32R.h"
#include "Core/ByteTool_C.h"
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

Bool Exporter::GUIPNGExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("PNG file (GDK)"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.png"));
		return true;
	}
	return false;
}

Bool Exporter::GUIPNGExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	UInt8 *tmpBuff;
	GdkPixbuf *image = (GdkPixbuf*)ToImage(pobj, tmpBuff).p;
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
		Optional<Media::RasterImage> srcImg = nullptr;
		NN<Media::RasterImage> nnimg;
		Media::ImageList *imgList;
		UInt8 *pngBuff;
		UIntOS pngSize;
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			imgList = (Media::ImageList*)pobj.Ptr();
			srcImg = imgList->GetImage(0, 0);
		}
		pngBuff = (UInt8*)buff;
		pngSize = buffSize;
		if (srcImg.SetTo(nnimg) && pngBuff[0] == 0x89 && pngBuff[1] == 0x50 && pngBuff[2] == 0x4e && pngBuff[3] == 0x47)
		{
			UInt8 tmpBuff[64];
			UInt32 chunkSize;
			Int32 chunkType;
			UIntOS i;
			stm->Write(Data::ByteArrayR(pngBuff, 8));
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
					UnsafeArray<const UInt8> iccBuff;
					if (nnimg->info.color.GetRAWICC().SetTo(iccBuff))
					{
						UInt32 iccSize = ReadMUInt32(&iccBuff[0]);
						UInt8 *chunkBuff = MemAlloc(UInt8, iccSize + 32);
						WriteInt32(&chunkBuff[4], ReadInt32("iCCP"));
						Text::StrConcatC((UTF8Char*)&chunkBuff[8], UTF8STRC("Photoshop ICC profile"));
						chunkBuff[30] = 0;

						UIntOS compSize = Data::Compress::Inflate::Compress(iccBuff, iccSize, &chunkBuff[31], true, Data::Compress::Inflate::CompressionLevel::BestCompression);
						if (compSize > 0)
						{
							WriteMUInt32(chunkBuff, 23 + compSize);
							WriteMUInt32(&chunkBuff[31 + compSize], crc.CalcDirect(&chunkBuff[4], 27 + compSize));
							stm->Write(Data::ByteArrayR(chunkBuff, 35 + compSize));
						}
						MemFree(chunkBuff);
					}
					else
					{
						if (nnimg->info.color.GetRTranParamRead()->GetTranType() == Media::CS::TRANT_sRGB)
						{
							WriteMUInt32(&tmpBuff[0], 1);
							WriteInt32(&tmpBuff[4], ReadInt32("sRGB"));
							tmpBuff[8] = 0;
							WriteMUInt32(&tmpBuff[9], crc.CalcDirect(&tmpBuff[4], 5));
							stm->Write(Data::ByteArrayR(tmpBuff, 13));
						}

						NN<const Media::ColorProfile::ColorPrimaries> prim = nnimg->info.color.GetPrimariesRead();
						WriteMUInt32(&tmpBuff[0], 32);
						WriteInt32(&tmpBuff[4], ReadInt32("cHRM"));
						WriteMInt32(&tmpBuff[8], Double2Int32(prim->w.x * 100000));
						WriteMInt32(&tmpBuff[12], Double2Int32(prim->w.y * 100000));
						WriteMInt32(&tmpBuff[16], Double2Int32(prim->r.x * 100000));
						WriteMInt32(&tmpBuff[20], Double2Int32(prim->r.y * 100000));
						WriteMInt32(&tmpBuff[24], Double2Int32(prim->g.x * 100000));
						WriteMInt32(&tmpBuff[28], Double2Int32(prim->g.y * 100000));
						WriteMInt32(&tmpBuff[32], Double2Int32(prim->b.x * 100000));
						WriteMInt32(&tmpBuff[36], Double2Int32(prim->b.y * 100000));
						WriteMUInt32(&tmpBuff[40], crc.CalcDirect(&tmpBuff[4], 36));
						stm->Write(Data::ByteArrayR(tmpBuff, 44));
					}
					if (nnimg->info.hdpi != 72.0 || nnimg->info.vdpi != 72.0)
					{
						Int32 hVal = Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / nnimg->info.hdpi));
						Int32 vVal = Double2Int32(1.0 / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_INCH, Math::Unit::Distance::DU_METER, 1.0 / nnimg->info.vdpi));
						WriteMInt32(&tmpBuff[0], 9);
						*(Int32*)&tmpBuff[4] = *(Int32*)"pHYs";
						WriteMInt32(&tmpBuff[8], hVal);
						WriteMInt32(&tmpBuff[12], vVal);
						tmpBuff[16] = 1;
						WriteMUInt32(&tmpBuff[17], crc.CalcDirect(&tmpBuff[4], 13));
						stm->Write(Data::ByteArrayR(tmpBuff, 21));
					}

					stm->Write(Data::ByteArrayR(&pngBuff[i], pngSize - i));
					break;
				}
				else
				{
					stm->Write(Data::ByteArrayR(&pngBuff[i], chunkSize + 12));
					i += chunkSize + 12;
				}
			}
		}
		else
		{
			stm->Write(Data::ByteArrayR(pngBuff, pngSize));
		}
		g_free(buff);
		return true;
	}
	return false;
}
