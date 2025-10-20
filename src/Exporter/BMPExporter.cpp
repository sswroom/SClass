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

IO::FileExporter::SupportType Exporter::BMPExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::RasterImage> img;
	if (!imgList->GetImage(0, 0).SetTo(img))
		return IO::FileExporter::SupportType::NotSupported;
	if (img->info.fourcc != 0)
		return IO::FileExporter::SupportType::NotSupported;
	if (img->info.pf == Media::PF_LE_A2B10G10R10)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_LE_R5G5B5)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_LE_R5G6B5)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_B8G8R8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_B8G8R8A8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_1)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_2)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_4)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_8)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_W1)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_W2)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_W4)
		return IO::FileExporter::SupportType::NormalStream;
	if (img->info.pf == Media::PF_PAL_W8)
		return IO::FileExporter::SupportType::NormalStream;
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::BMPExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Bitmap Image"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.bmp"));
		return true;
	}
	return false;
}

void Exporter::BMPExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::BMPExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (IsObjectSupported(pobj) == SupportType::NotSupported)
		return false;
	NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	NN<Media::RasterImage> img;
	if (!imgList->GetImage(0, 0).SetTo(img))
		return false;

	UInt8 buff[138];
	UOSInt hdrSize = 54;
	UOSInt lineSize = img->info.storeSize.x * img->info.storeBPP;
	if (lineSize & 7)
		lineSize = lineSize + 8 - (lineSize & 7);
	lineSize = lineSize >> 3;
	if (lineSize & 3)
		lineSize = lineSize + 4 - (lineSize & 3);
	UOSInt palSize = 0;
	if (img->info.storeBPP <= 8)
		palSize = (UOSInt)4 << img->info.storeBPP;
	UOSInt iccSize = 0;
	UnsafeArray<const UInt8> rawICC;
	if (img->info.color.GetRAWICC().SetTo(rawICC))
	{
		hdrSize = 124;
		iccSize = ReadMUInt32(&rawICC[0]);
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
		WriteInt32(&buff[126], (Int32)(hdrSize + palSize + (img->info.dispSize.y * lineSize)));
		WriteInt32(&buff[130], (Int32)iccSize);
		WriteInt32(&buff[134], 0);
	}
	else
	{
		Media::ColorProfile::ColorType ct = img->info.color.GetPrimaries()->colorType;
		if (ct == Media::ColorProfile::CT_PUNKNOWN || ct == Media::ColorProfile::CT_VUNKNOWN)
		{
			if (img->info.pf == Media::PF_LE_R5G6B5 || img->info.pf == Media::PF_LE_A2B10G10R10)
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
		else if (ct == Media::ColorProfile::CT_SRGB && img->info.color.rtransfer.GetTranType() == Media::CS::TRANT_sRGB)
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
			WriteInt32(&buff[126], (Int32)(hdrSize + palSize + (img->info.dispSize.y * lineSize)));
			WriteInt32(&buff[130], (Int32)iccSize);
			WriteInt32(&buff[134], 0);
		}
		else
		{
			
			NN<Media::ColorProfile::ColorPrimaries> primaries = img->info.color.GetPrimaries();
			Math::Vector3 xyzVec;
			hdrSize = 124;
			WriteInt32(&buff[70], 0); //bV5CSType = LCS_CALIBRATED_RGB
			xyzVec.val[0] = primaries->r.x;
			xyzVec.val[1] = primaries->r.y;
			xyzVec.val[2] = 1.0 - primaries->r.x - primaries->r.y;
			WriteInt32(&buff[74], Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[78], Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[82], Double2Int32(xyzVec.val[2] * 0x40000000));

			xyzVec.val[0] = primaries->g.x;
			xyzVec.val[1] = primaries->g.y;
			xyzVec.val[2] = 1.0 - primaries->g.x - primaries->g.y;
			WriteInt32(&buff[86], Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[90], Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[94], Double2Int32(xyzVec.val[2] * 0x40000000));

			xyzVec.val[0] = primaries->b.x;
			xyzVec.val[1] = primaries->b.y;
			xyzVec.val[2] = 1.0 - primaries->b.x - primaries->b.y;
			WriteInt32(&buff[98], Double2Int32(xyzVec.val[0] * 0x40000000));
			WriteInt32(&buff[102], Double2Int32(xyzVec.val[1] * 0x40000000));
			WriteInt32(&buff[106], Double2Int32(xyzVec.val[2] * 0x40000000));

			WriteInt32(&buff[110], Double2Int32(img->info.color.rtransfer.GetGamma() * 65536));
			WriteInt32(&buff[114], Double2Int32(img->info.color.gtransfer.GetGamma() * 65536));
			WriteInt32(&buff[118], Double2Int32(img->info.color.btransfer.GetGamma() * 65536));

			WriteInt32(&buff[122], 8); //LCS_GM_ABS_COLORIMETRIC
			WriteInt32(&buff[126], 0);
			WriteInt32(&buff[130], 0);
			WriteInt32(&buff[134], 0);
		}
	}
	buff[0] = 'B';
	buff[1] = 'M';
	WriteInt32(&buff[2], (Int32)(hdrSize + 14 + palSize + (img->info.dispSize.y * lineSize) + iccSize));
	WriteInt32(&buff[6], 0);
	WriteInt32(&buff[10], (Int32)(hdrSize + 14 + palSize));
	WriteInt32(&buff[14], (Int32)hdrSize);
	WriteInt32(&buff[18], (Int32)(img->info.dispSize.x));
	WriteInt32(&buff[22], (Int32)(img->info.dispSize.y));
	WriteInt32(&buff[26], 1);
	WriteUInt32(&buff[28], img->info.storeBPP);
	WriteInt32(&buff[30], (img->info.pf == Media::PF_LE_R5G6B5 || img->info.pf == Media::PF_LE_A2B10G10R10)?3:0);
	WriteInt32(&buff[34], 0);
	WriteInt32(&buff[38], Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, img->info.hdpi)));
	WriteInt32(&buff[42], Double2Int32(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, Math::Unit::Distance::DU_INCH, img->info.vdpi)));
	if (img->info.storeBPP <= 8)
	{
		WriteInt32(&buff[46], 1 << img->info.storeBPP);
	}
	else
	{
		WriteInt32(&buff[46], 0);
	}
	WriteInt32(&buff[50], 0);
	if (img->info.pf == Media::PF_LE_R5G6B5)
	{
		WriteInt32(&buff[54], 0xf800);
		WriteInt32(&buff[58], 0x07e0);
		WriteInt32(&buff[62], 0x001f);
		WriteInt32(&buff[66], 0x0000);
	}
	else if (img->info.pf == Media::PF_LE_A2B10G10R10)
	{
		WriteInt32(&buff[54], 0x000003ff);
		WriteInt32(&buff[58], 0x000ffc00);
		WriteInt32(&buff[62], 0x3ff00000);
		if (img->info.atype == Media::AT_ALPHA)
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

	stm->Write(Data::ByteArrayR(buff, hdrSize + 14));
	UnsafeArray<UInt8> pal;
	if (img->info.storeBPP <= 8 && img->pal.SetTo(pal))
	{
		stm->Write(Data::ByteArrayR(pal, palSize));
	}

	UInt8 *imgData = MemAlloc(UInt8, lineSize * img->info.dispSize.y);
	img->GetRasterData(imgData, 0, 0, img->info.dispSize.x, img->info.dispSize.y, lineSize, true, Media::RotateType::None);
	stm->Write(Data::ByteArrayR(imgData, lineSize * img->info.dispSize.y));

	if (iccSize > 0)
	{
		stm->Write(Data::ByteArrayR(rawICC, iccSize));
	}
	MemFree(imgData);
	return true;
}
