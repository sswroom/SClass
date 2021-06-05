#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/BMPExporter.h"
#include "Math/Math.h"
#include "Math/Unit/Distance.h"
#include "Media/ImageList.h"
#include "Text/MyString.h"

Exporter::BMPExporter::BMPExporter()
{
}

Exporter::BMPExporter::~BMPExporter()
{
}

Int32 Exporter::BMPExporter::GetName()
{
	return *(Int32*)"BMPE";
}

IO::FileExporter::SupportType Exporter::BMPExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	if (imgList->GetCount() != 1)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::Image *img = imgList->GetImage(0, &imgTime);
	if (img->info->fourcc != 0)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	if (img->info->pf == Media::PF_LE_A2B10G10R10)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_LE_R5G5B5)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_LE_R5G6B5)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_B8G8R8)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_B8G8R8A8)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_1)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_2)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_4)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_8)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_W1)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_W2)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_W4)
		return IO::FileExporter::ST_NORMAL_STREAM;
	if (img->info->pf == Media::PF_PAL_W8)
		return IO::FileExporter::ST_NORMAL_STREAM;
	return IO::FileExporter::ST_NOT_SUPPORTED;
}

Bool Exporter::BMPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Bitmap Image");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.bmp");
		return true;
	}
	return false;
}

void Exporter::BMPExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::BMPExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (!IsObjectSupported(pobj))
		return false;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	Media::Image *img = imgList->GetImage(0, &imgTime);

	UInt8 buff[138];
	UOSInt hdrSize = 54;
	UOSInt lineSize = img->info->storeWidth * img->info->storeBPP;
	if (lineSize & 7)
		lineSize = lineSize + 8 - (lineSize & 7);
	lineSize = lineSize >> 3;
	if (lineSize & 3)
		lineSize = lineSize + 4 - (lineSize & 3);
	UOSInt palSize = 0;
	if (img->info->storeBPP <= 8)
		palSize = (UOSInt)4 << img->info->storeBPP;
	const UInt8 *rawICC = img->info->color->GetRAWICC();
	UOSInt iccSize = 0;
	if (rawICC)
	{
		hdrSize = 124;
		iccSize = ReadMUInt32(rawICC);
		WriteInt32(&buff[70], ReadInt32((const UInt8*)"DEBM")); //bV5CSType = PROFILE_EMBEDDED
		WriteInt32(&buff[74], 0);
		WriteInt32(&buff[78], 0);
		WriteInt32(&buff[82], 0);
		WriteInt32(&buff[86], 0);
		WriteInt32(&buff[90], 0);
		WriteInt32(&buff[94], 0);
		WriteInt32(&buff[98], 0);
		WriteInt32(&buff[102], 0);
		WriteInt32(&buff[106], 0);
		WriteInt32(&buff[110], 0);
		WriteInt32(&buff[114], 0);
		WriteInt32(&buff[118], 0);
		WriteInt32(&buff[122], 0);
		WriteInt32(&buff[126], (Int32)(hdrSize + palSize + (img->info->dispHeight * lineSize)));
		WriteInt32(&buff[130], (Int32)iccSize);
		WriteInt32(&buff[134], 0);
	}
	else
	{
		Media::ColorProfile::ColorType ct = img->info->color->GetPrimaries()->colorType;
		if (ct == Media::ColorProfile::CT_PUNKNOWN || ct == Media::ColorProfile::CT_VUNKNOWN)
		{
			if (img->info->pf == Media::PF_LE_R5G6B5 || img->info->pf == Media::PF_LE_A2B10G10R10)
			{
				hdrSize = 108; //BITMAPV4INFOHEADER
				WriteInt32(&buff[70], 0);
				WriteInt32(&buff[74], 0);
				WriteInt32(&buff[78], 0);
				WriteInt32(&buff[82], 0);
				WriteInt32(&buff[86], 0);
				WriteInt32(&buff[90], 0);
				WriteInt32(&buff[94], 0);
				WriteInt32(&buff[98], 0);
				WriteInt32(&buff[102], 0);
				WriteInt32(&buff[106], 0);
				WriteInt32(&buff[110], 0);
				WriteInt32(&buff[114], 0);
				WriteInt32(&buff[118], 0);
			}
		}
		else if (ct == Media::ColorProfile::CT_SRGB && img->info->color->rtransfer->GetTranType() == Media::CS::TRANT_sRGB)
		{
			hdrSize = 124;
			WriteInt32(&buff[70], ReadInt32((const UInt8*)"BGRs")); //bV5CSType = LCS_sRGB
			WriteInt32(&buff[74], 0);
			WriteInt32(&buff[78], 0);
			WriteInt32(&buff[82], 0);
			WriteInt32(&buff[86], 0);
			WriteInt32(&buff[90], 0);
			WriteInt32(&buff[94], 0);
			WriteInt32(&buff[98], 0);
			WriteInt32(&buff[102], 0);
			WriteInt32(&buff[106], 0);
			WriteInt32(&buff[110], 0);
			WriteInt32(&buff[114], 0);
			WriteInt32(&buff[118], 0);
			WriteInt32(&buff[122], 0);
			WriteInt32(&buff[126], (Int32)(hdrSize + palSize + (img->info->dispHeight * lineSize)));
			WriteInt32(&buff[130], (Int32)iccSize);
			WriteInt32(&buff[134], 0);
		}
		else
		{
			
			Media::ColorProfile::ColorPrimaries *primaries = img->info->color->GetPrimaries();
			Math::Vector3 xyzVec;
			hdrSize = 124;
			WriteInt32(&buff[70], 0); //bV5CSType = LCS_CALIBRATED_RGB
			xyzVec.val[0] = primaries->rx;
			xyzVec.val[1] = primaries->ry;
			xyzVec.val[2] = 1.0 - primaries->rx - primaries->ry;
			WriteInt32(&buff[74], Math::Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[78], Math::Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[82], Math::Double2Int32(xyzVec.val[2] * 0x40000000));

			xyzVec.val[0] = primaries->gx;
			xyzVec.val[1] = primaries->gy;
			xyzVec.val[2] = 1.0 - primaries->gx - primaries->gy;
			WriteInt32(&buff[86], Math::Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[90], Math::Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[94], Math::Double2Int32(xyzVec.val[2] * 0x40000000));

			xyzVec.val[0] = primaries->bx;
			xyzVec.val[1] = primaries->by;
			xyzVec.val[2] = 1.0 - primaries->bx - primaries->by;
			WriteInt32(&buff[98], Math::Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[102], Math::Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[106], Math::Double2Int32(xyzVec.val[2] * 0x40000000));

			WriteInt32(&buff[110], Math::Double2Int32(img->info->color->rtransfer->GetGamma() * 65536));
			WriteInt32(&buff[114], Math::Double2Int32(img->info->color->gtransfer->GetGamma() * 65536));
			WriteInt32(&buff[118], Math::Double2Int32(img->info->color->btransfer->GetGamma() * 65536));

			WriteInt32(&buff[122], 8); //LCS_GM_ABS_COLORIMETRIC
			WriteInt32(&buff[126], 0);
			WriteInt32(&buff[130], 0);
			WriteInt32(&buff[134], 0);
		}
	}
	buff[0] = 'B';
	buff[1] = 'M';
	WriteInt32(&buff[2], (Int32)(hdrSize + 14 + palSize + (img->info->dispHeight * lineSize) + iccSize));
	WriteInt32(&buff[6], 0);
	WriteInt32(&buff[10], (Int32)(hdrSize + 14 + palSize));
	WriteInt32(&buff[14], (Int32)hdrSize);
	WriteInt32(&buff[18], (Int32)(img->info->dispWidth));
	WriteInt32(&buff[22], (Int32)(img->info->dispHeight));
	WriteInt32(&buff[26], 1);
	WriteUInt32(&buff[28], img->info->storeBPP);
	WriteInt32(&buff[30], (img->info->pf == Media::PF_LE_R5G6B5 || img->info->pf == Media::PF_LE_A2B10G10R10)?3:0);
	WriteInt32(&buff[34], 0);
	WriteInt32(&buff[38], Math::Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, img->info->hdpi)));
	WriteInt32(&buff[42], Math::Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, img->info->vdpi)));
	if (img->info->storeBPP <= 8)
	{
		WriteInt32(&buff[46], 1 << img->info->storeBPP);
	}
	else
	{
		WriteInt32(&buff[46], 0);
	}
	WriteInt32(&buff[50], 0);
	if (img->info->pf == Media::PF_LE_R5G6B5)
	{
		WriteInt32(&buff[54], 0xf800);
		WriteInt32(&buff[58], 0x07e0);
		WriteInt32(&buff[62], 0x001f);
		WriteInt32(&buff[66], 0x0000);
	}
	else if (img->info->pf == Media::PF_LE_A2B10G10R10)
	{
		WriteInt32(&buff[54], 0x000003ff);
		WriteInt32(&buff[58], 0x000ffc00);
		WriteInt32(&buff[62], 0x3ff00000);
		if (img->info->atype == Media::AT_ALPHA)
		{
			WriteUInt32(&buff[66], 0xc0000000);
		}
		else
		{
			WriteUInt32(&buff[66], 0x00000000);
		}
	}
	else
	{
		WriteInt32(&buff[54], 0);
		WriteInt32(&buff[58], 0);
		WriteInt32(&buff[62], 0);
		WriteInt32(&buff[66], 0);
	}

	stm->Write(buff, hdrSize + 14);
	if (img->info->storeBPP <= 8)
	{
		stm->Write(img->pal, palSize);
	}

	UInt8 *imgData = MemAlloc(UInt8, lineSize * img->info->dispHeight);
	img->GetImageData(imgData + lineSize * (img->info->dispHeight - 1), 0, 0, img->info->dispWidth, img->info->dispHeight, -(OSInt)lineSize);
	stm->Write(imgData, lineSize * img->info->dispHeight);

	if (iccSize > 0)
	{
		stm->Write(rawICC, iccSize);
	}
	MemFree(imgData);
	return true;
}
