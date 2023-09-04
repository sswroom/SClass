#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/TIFFExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Text/MyString.h"

void Exporter::TIFFExporter::GenSubExifBuff(IO::SeekableStream *stm, UInt64 buffOfst, Media::EXIFData *exif)
{
	UInt64 currOfst = buffOfst;
	Data::ArrayListUInt32 ids;
	UOSInt i;
	UOSInt j;
	UInt8 *ifd;
	UInt32 exifId;
	Media::EXIFData::EXIFItem *exifItem;
	exif->GetExifIds(&ids);
	i = 0;
	j = ids.GetCount();
	ifd = MemAlloc(UInt8, 12 * j + 6);
	WriteInt16(ifd, (Int16)j);
	stm->Write(ifd, 12 * j + 6);
	currOfst += 12 * j + 6;
	while (i < j)
	{
		exifId = ids.GetItem(i);
		exifItem = exif->GetExifItem(exifId);
		WriteInt16(&ifd[2 + i * 12], exifItem->id);
		WriteUInt32(&ifd[6 + i * 12], (UInt32)exifItem->cnt);
		switch (exifItem->type)
		{
		case Media::EXIFData::ET_BYTES:
			WriteInt16(&ifd[4 + i * 12], 1);
			if (exifItem->cnt <= 4)
			{
				WriteInt32(&ifd[10 + i * 12], exifItem->value);
			}
			else
			{
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt);
				currOfst += exifItem->cnt;
			}
			break;
		case Media::EXIFData::ET_STRING:
			WriteInt16(&ifd[4 + i * 12], 2);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt);
			currOfst += exifItem->cnt;
			break;
		case Media::EXIFData::ET_UINT16:
			WriteInt16(&ifd[4 + i * 12], 3);
			if (exifItem->cnt <= 2)
			{
				WriteInt32(&ifd[10 + i * 12], exifItem->value);
			}
			else
			{
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 2);
				currOfst += exifItem->cnt * 2;
			}
			break;
		case Media::EXIFData::ET_UINT32:
			WriteInt16(&ifd[4 + i * 12], 4);
			if (exifItem->cnt == 1)
			{
				WriteInt32(&ifd[10 + i * 12], exifItem->value);
			}
			else
			{
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 4);
				currOfst += exifItem->cnt * 4;
			}
			break;
		case Media::EXIFData::ET_RATIONAL:
			WriteInt16(&ifd[4 + i * 12], 5);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 8);
			currOfst += exifItem->cnt * 8;
			break;
		case Media::EXIFData::ET_OTHER:
			WriteInt16(&ifd[4 + i * 12], 7);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt);
			currOfst += exifItem->cnt;
			break;
		case Media::EXIFData::ET_INT16:
			WriteInt16(&ifd[4 + i * 12], 8);
			if (exifItem->cnt <= 2)
			{
				WriteInt32(&ifd[10 + i * 12], exifItem->value);
			}
			else
			{
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 2);
				currOfst += exifItem->cnt * 2;
			}
			break;
		case Media::EXIFData::ET_INT32:
			WriteInt16(&ifd[4 + i * 12], 9);
			if (exifItem->cnt <= 1)
			{
				WriteInt32(&ifd[10 + i * 12], exifItem->value);
			}
			else
			{
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 4);
				currOfst += exifItem->cnt * 4;
			}
			break;
		case Media::EXIFData::ET_SUBEXIF:
			WriteInt16(&ifd[4 + i * 12], 4);
			WriteInt32(&ifd[6 + i * 12], 1);

			{
				IO::MemoryStream mstm;
				UOSInt buffSize;
				UInt8 *mbuff;
				GenSubExifBuff(&mstm, currOfst, ((Media::EXIFData*)exifItem->dataBuff));
				mbuff = mstm.GetBuff(&buffSize);
				WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
				stm->Write(mbuff, buffSize);
				currOfst += buffSize;
			}
			break;
		case Media::EXIFData::ET_SRATIONAL:
			WriteInt16(&ifd[4 + i * 12], 10);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 8);
			currOfst += exifItem->cnt * 8;
			break;
		case Media::EXIFData::ET_DOUBLE:
			WriteInt16(&ifd[4 + i * 12], 12);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 8);
			currOfst += exifItem->cnt * 8;
			break;
		case Media::EXIFData::ET_UINT64:
			WriteInt16(&ifd[4 + i * 12], 16);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 8);
			currOfst += exifItem->cnt * 8;
			break;
		case Media::EXIFData::ET_INT64:
			WriteInt16(&ifd[4 + i * 12], 17);
			WriteUInt32(&ifd[10 + i * 12], (UInt32)currOfst);
			stm->Write((UInt8*)exifItem->dataBuff, exifItem->cnt * 8);
			currOfst += exifItem->cnt * 8;
			break;
		case Media::EXIFData::ET_UNKNOWN:
			break;
		}
		i++;
	}
	WriteInt32(&ifd[2 + j * 12], 0);
	stm->SeekFromBeginning(0);
	stm->Write(ifd, 6 + j * 12);
	MemFree(ifd);
}

Exporter::TIFFExporter::TIFFExporter()
{
}

Exporter::TIFFExporter::~TIFFExporter()
{
}

Int32 Exporter::TIFFExporter::GetName()
{
	return *(Int32*)"TIFE";
}

IO::FileExporter::SupportType Exporter::TIFFExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	Media::Image *img = imgList->GetImage(0, &imgTime);
	if (img->info.fourcc == 0)
	{
		if (img->info.pf == Media::PF_PAL_W1)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_W2)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_W4)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_1)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_2)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_4)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_8)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_B8G8R8A8)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_B8G8R8)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_W8A8)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_PAL_W8)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_B16G16R16A16)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_B16G16R16)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_W16A16)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_W16)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_FB32G32R32A32)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_FB32G32R32)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_FW32A32)
			return IO::FileExporter::SupportType::NormalStream;
		if (img->info.pf == Media::PF_LE_FW32)
			return IO::FileExporter::SupportType::NormalStream;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::TIFFExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("TIFF Image"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.tif"));
		return true;
	}
	return false;
}

void Exporter::TIFFExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::TIFFExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return false;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	UInt8 buff[54];

	buff[0] = 'I';
	buff[1] = 'I';
	WriteInt16(&buff[2], 42);
	WriteInt32(&buff[4], 8);
	stm->Write(buff, 8);

	UInt64 currOfst = 8;
	UInt64 lastOfst = 4;
	UOSInt i;
	UOSInt j = imgList->GetCount();
	i = 0;
	while (i < j)
	{
		Media::Image *img = imgList->GetImage(i, &imgTime);
		UInt32 stripCnt = 1;
		i++;

		if (img->info.fourcc != 0)
			continue;


		Media::EXIFData *newExif;
		if (img->exif)
		{
			newExif = img->exif->Clone();
		}
		else
		{
			NEW_CLASS(newExif, Media::EXIFData(Media::EXIFData::EM_STANDARD));
		}
		UInt32 ibuff[4];
		UInt16 sibuff[4];
		if (img->info.pf == Media::PF_PAL_W1)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 1;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_W2)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 2;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_W4)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 4;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_1)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 1;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 3;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			UInt8 *colorTable = MemAlloc(UInt8, 2 * 6);
			UInt8 *srcPtr = img->pal;
			UInt8 *destPtr = colorTable;
			OSInt k;
			k = 2;
			while (k-- > 0)
			{
				destPtr[0] = srcPtr[2];
				destPtr[1] = srcPtr[2];
				destPtr[4 + 0] = srcPtr[1];
				destPtr[4 + 1] = srcPtr[1];
				destPtr[8 + 0] = srcPtr[0];
				destPtr[8 + 1] = srcPtr[0];
				srcPtr += 4;
				destPtr += 2;
			}
			newExif->AddUInt16(320, 2 * 3, (UInt16*)colorTable);
			MemFree(colorTable);
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_2)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 2;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 3;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			UInt8 *colorTable = MemAlloc(UInt8, 4 * 6);
			UInt8 *srcPtr = img->pal;
			UInt8 *destPtr = colorTable;
			OSInt k;
			k = 4;
			while (k-- > 0)
			{
				destPtr[0] = srcPtr[2];
				destPtr[1] = srcPtr[2];
				destPtr[8 + 0] = srcPtr[1];
				destPtr[8 + 1] = srcPtr[1];
				destPtr[16 + 0] = srcPtr[0];
				destPtr[16 + 1] = srcPtr[0];
				srcPtr += 4;
				destPtr += 2;
			}
			newExif->AddUInt16(320, 4 * 3, (UInt16*)colorTable);
			MemFree(colorTable);
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_4)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 4;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 3;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			UInt8 *colorTable = MemAlloc(UInt8, 16 * 6);
			UInt8 *srcPtr = img->pal;
			UInt8 *destPtr = colorTable;
			OSInt k;
			k = 16;
			while (k-- > 0)
			{
				destPtr[0] = srcPtr[2];
				destPtr[1] = srcPtr[2];
				destPtr[32 + 0] = srcPtr[1];
				destPtr[32 + 1] = srcPtr[1];
				destPtr[64 + 0] = srcPtr[0];
				destPtr[64 + 1] = srcPtr[0];
				srcPtr += 4;
				destPtr += 2;
			}
			newExif->AddUInt16(320, 16 * 3, (UInt16*)colorTable);
			MemFree(colorTable);
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_8)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 8;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 3;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			UInt8 *colorTable = MemAlloc(UInt8, 256 * 6);
			UInt8 *srcPtr = img->pal;
			UInt8 *destPtr = colorTable;
			OSInt k;
			k = 256;
			while (k-- > 0)
			{
				destPtr[0] = srcPtr[2];
				destPtr[1] = srcPtr[2];
				destPtr[512 + 0] = srcPtr[1];
				destPtr[512 + 1] = srcPtr[1];
				destPtr[1024 + 0] = srcPtr[0];
				destPtr[1024 + 1] = srcPtr[0];
				srcPtr += 4;
				destPtr += 2;
			}
			newExif->AddUInt16(320, 256 * 3, (UInt16*)colorTable);
			MemFree(colorTable);
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_B8G8R8A8)
		{
			sibuff[0] = 4;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 8;
			sibuff[1] = 8;
			sibuff[2] = 8;
			sibuff[3] = 8;
			newExif->AddUInt16(258, 4, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_B8G8R8)
		{
			sibuff[0] = 3;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 8;
			sibuff[1] = 8;
			sibuff[2] = 8;
			newExif->AddUInt16(258, 3, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 2, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_W8A8)
		{
			sibuff[0] = 2;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 8;
			sibuff[1] = 8;
			newExif->AddUInt16(258, 2, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_PAL_W8)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 8;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_B16G16R16A16)
		{
			sibuff[0] = 4;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 16;
			sibuff[1] = 16;
			sibuff[2] = 16;
			sibuff[3] = 16;
			newExif->AddUInt16(258, 4, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_B16G16R16)
		{
			sibuff[0] = 3;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 16;
			sibuff[1] = 16;
			sibuff[2] = 16;
			newExif->AddUInt16(258, 3, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_W16A16)
		{
			sibuff[0] = 2;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 16;
			sibuff[1] = 16;
			newExif->AddUInt16(258, 2, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_W16)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 16;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			newExif->Remove(339); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_FB32G32R32A32)
		{
			sibuff[0] = 4;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 32;
			sibuff[1] = 32;
			sibuff[2] = 32;
			sibuff[3] = 32;
			newExif->AddUInt16(258, 4, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			sibuff[0] = 3;
			sibuff[1] = 3;
			sibuff[2] = 3;
			sibuff[3] = 3;
			newExif->AddUInt16(339, 4, sibuff); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_FB32G32R32)
		{
			sibuff[0] = 3;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 32;
			sibuff[1] = 32;
			sibuff[2] = 32;
			newExif->AddUInt16(258, 3, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 2;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			sibuff[0] = 3;
			sibuff[1] = 3;
			sibuff[2] = 3;
			newExif->AddUInt16(339, 3, sibuff); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_FW32A32)
		{
			sibuff[0] = 2;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 32;
			sibuff[1] = 32;
			newExif->AddUInt16(258, 2, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			sibuff[0] = 1;
			newExif->AddUInt16(284, 1, sibuff); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			sibuff[0] = 2;
			newExif->AddUInt16(338, 1, sibuff); //ExtraSamples
			sibuff[0] = 3;
			sibuff[1] = 3;
			newExif->AddUInt16(339, 2, sibuff); //SampleFormat
		}
		else if (img->info.pf == Media::PF_LE_FW32)
		{
			sibuff[0] = 1;
			newExif->AddUInt16(277, 1, sibuff); //SamplesPerPixel
			sibuff[0] = 32;
			newExif->AddUInt16(258, 1, sibuff); //BitPerSample (R, G, B)
			sibuff[0] = 1;
			newExif->AddUInt16(262, 1, sibuff); //PhotometricInterpretation
			newExif->Remove(284); //PlanarConfiguration
			newExif->Remove(320); //ColorMap
			newExif->Remove(338); //ExtraSamples
			sibuff[0] = 3;
			newExif->AddUInt16(339, 1, sibuff); //SampleFormat
		}
		else
		{
			DEL_CLASS(newExif);
			continue;
		}
		ibuff[0] = (UInt32)img->info.dispSize.x;
		newExif->AddUInt32(256, 1, ibuff); //Width
		ibuff[0] = (UInt32)img->info.dispSize.y;
		newExif->AddUInt32(257, 1, ibuff); //Height
		sibuff[0] = 1;
		newExif->AddUInt16(259, 1, sibuff); //Compression

		ibuff[0] = 0;
		newExif->AddUInt32(273, 1, ibuff); //StripOffsets
		if (img->info.storeBPP < 8)
		{
			UOSInt lineSize = (img->info.dispSize.x * img->info.storeBPP + 7) >> 3;
			ibuff[0] = (UInt32)(lineSize * img->info.dispSize.y);
		}
		else
		{
			ibuff[0] = (UInt32)(img->info.dispSize.x * img->info.dispSize.y * img->info.storeBPP >> 3);
		}
		newExif->AddUInt32(279, 1, ibuff); //StripByteCounts
		ibuff[0] = (UInt32)img->info.dispSize.y;
		newExif->AddUInt32(278, 1, ibuff); //RowsPerStrip
		stripCnt = 1;

		ibuff[0] = (UInt32)Double2Int32(img->info.hdpi * 100);
		ibuff[1] = 100;
		newExif->AddRational(282, 1, ibuff); //XResolution
		ibuff[0] = (UInt32)Double2Int32(img->info.vdpi * 100);
		ibuff[1] = 100;
		newExif->AddRational(283, 1, ibuff); //YResolution
		sibuff[0] = 2;
		newExif->AddUInt16(296, 1, sibuff); //ResolutionUnit
		newExif->Remove(317); //Predictor
		const UInt8 *rawICC = img->info.color.GetRAWICC();
		if (rawICC)
		{
			newExif->AddOther(34675, ReadMUInt32(rawICC), rawICC);
		}
		else
		{
			newExif->Remove(34675);
		}

		UInt64 ifdOfst = currOfst;
		Data::ArrayListUInt32 ids;
		UOSInt k;
		UOSInt l;
		UInt8 *ifd;
		UInt64 stripOfst = 0;
		UInt64 stripCntOfst = 0;
		UInt32 exifId;
		Media::EXIFData::EXIFItem *exifItem;
		ids.Clear();
		newExif->GetExifIds(&ids);
		k = 0;
		l = ids.GetCount();
		ifd = MemAlloc(UInt8, 12 * l + 6);
		WriteInt16(ifd, (Int16)l);
		stm->Write(ifd, 12 * l + 6);
		currOfst += 12 * l + 6;
		while (k < l)
		{
			exifId = ids.GetItem(k);
			exifItem = newExif->GetExifItem(exifId);
			if (exifId == 273)
			{
				if (stripCnt == 1)
				{
					stripOfst = 10 + k * 12;
				}
				else
				{
					stripOfst = currOfst;
				}
			}
			else if (exifId == 279)
			{
				if (stripCnt == 1)
				{
					stripCntOfst = 10 + k * 12;
				}
				else
				{
					stripCntOfst = currOfst;
				}
			}
			else if (exifId == 700)
			{
				exifId = exifItem->id;
			}
			WriteInt16(&ifd[2 + k * 12], exifItem->id);
			WriteUInt32(&ifd[6 + k * 12], (UInt32)exifItem->cnt);
			switch (exifItem->type)
			{
			case Media::EXIFData::ET_BYTES:
				WriteInt16(&ifd[4 + k * 12], 1);
				if (exifItem->cnt <= 4)
				{
					WriteInt32(&ifd[10 + k * 12], exifItem->value);
				}
				else
				{
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt);
					currOfst += exifItem->cnt;
				}
				break;
			case Media::EXIFData::ET_STRING:
				WriteInt16(&ifd[4 + k * 12], 2);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt);
				currOfst += exifItem->cnt;
				break;
			case Media::EXIFData::ET_UINT16:
				WriteInt16(&ifd[4 + k * 12], 3);
				if (exifItem->cnt == 2)
				{
					WriteInt32(&ifd[10 + k * 12], exifItem->value);
				}
				else if (exifItem->cnt == 1)
				{
					WriteInt16(&ifd[10 + k * 12], ReadInt16((UInt8*)&exifItem->value));
					WriteInt16(&ifd[12 + k * 12], 0);
				}
				else
				{
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 2);
					currOfst += exifItem->cnt * 2;
				}
				break;
			case Media::EXIFData::ET_UINT32:
				WriteInt16(&ifd[4 + k * 12], 4);
				if (exifItem->cnt == 1)
				{
					WriteInt32(&ifd[10 + k * 12], exifItem->value);
				}
				else
				{
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 4);
					currOfst += exifItem->cnt * 4;
				}
				break;
			case Media::EXIFData::ET_RATIONAL:
				WriteInt16(&ifd[4 + k * 12], 5);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 8);
				currOfst += exifItem->cnt * 8;
				break;
			case Media::EXIFData::ET_OTHER:
				WriteInt16(&ifd[4 + k * 12], 7);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt);
				currOfst += exifItem->cnt;
				break;
			case Media::EXIFData::ET_INT16:
				WriteInt16(&ifd[4 + k * 12], 8);
				if (exifItem->cnt <= 2)
				{
					WriteInt32(&ifd[10 + k * 12], exifItem->value);
				}
				else
				{
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 2);
					currOfst += exifItem->cnt * 2;
				}
				break;
			case Media::EXIFData::ET_INT32:
				WriteInt16(&ifd[4 + k * 12], 9);
				if (exifItem->cnt <= 1)
				{
					WriteInt32(&ifd[10 + k * 12], exifItem->value);
				}
				else
				{
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 4);
					currOfst += exifItem->cnt * 4;
				}
				break;
			case Media::EXIFData::ET_SUBEXIF:
				WriteInt16(&ifd[4 + k * 12], 4);
				WriteInt32(&ifd[6 + k * 12], 1);

				{
					IO::MemoryStream mstm;
					UOSInt buffSize;
					UInt8 *mbuff;
					GenSubExifBuff(&mstm, currOfst, ((Media::EXIFData*)exifItem->dataBuff));
					mbuff = mstm.GetBuff(&buffSize);
					WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
					stm->Write(mbuff, buffSize);
					currOfst += buffSize;
				}
				break;
			case Media::EXIFData::ET_SRATIONAL:
				WriteInt16(&ifd[4 + k * 12], 10);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 8);
				currOfst += exifItem->cnt * 8;
				break;
			case Media::EXIFData::ET_DOUBLE:
				WriteInt16(&ifd[4 + k * 12], 12);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 8);
				currOfst += exifItem->cnt * 8;
				break;
			case Media::EXIFData::ET_UINT64:
				WriteInt16(&ifd[4 + k * 12], 16);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 8);
				currOfst += exifItem->cnt * 8;
				break;
			case Media::EXIFData::ET_INT64:
				WriteInt16(&ifd[4 + k * 12], 17);
				WriteUInt32(&ifd[10 + k * 12], (UInt32)currOfst);
				stm->Write((UInt8*)exifItem->dataBuff, (UOSInt)exifItem->cnt * 8);
				currOfst += exifItem->cnt * 8;
				break;
			case Media::EXIFData::ET_UNKNOWN:
				break;
			}
			if (currOfst & 1)
			{
				buff[0] = 0;
				stm->Write(buff, 1);
				currOfst += 1;
			}
			k++;
		}

		if (stripOfst != 0 && stripCntOfst != 0)
		{
			if (stripCnt == 1)
			{
				WriteUInt32(&ifd[stripOfst], (UInt32)currOfst);
			}
			else
			{
				UInt32 currSOfst = (UInt32)currOfst;
				UInt32 sofstStep = (UInt32)(img->info.dispSize.x * (img->info.storeBPP >> 3) * 10);
				UInt32 *stripBuff;
				UInt32 *stripCntBuff;
				UOSInt sizeLeft = img->info.dispSize.x * img->info.dispSize.y * (img->info.storeBPP >> 3);
				stripBuff = MemAlloc(UInt32, stripCnt);
				stripCntBuff = MemAlloc(UInt32, stripCnt);
				k = 0;
				while (k < stripCnt)
				{
					stripBuff[k] = currSOfst;
					if (sizeLeft < sofstStep)
					{
						stripCntBuff[k] = (UInt32)sizeLeft;
						sizeLeft = 0;
					}
					else
					{
						stripCntBuff[k] = sofstStep;
						sizeLeft -= sofstStep;
					}
					currSOfst += sofstStep;
					k++;
				}
				stm->SeekFromBeginning(stripOfst);
				stm->Write((UInt8*)stripBuff, stripCnt * 4);
				stm->SeekFromBeginning(stripCntOfst);
				stm->Write((UInt8*)stripCntBuff, stripCnt * 4);
				MemFree(stripBuff);
				MemFree(stripCntBuff);
			}
		}
		stm->SeekFromBeginning(currOfst);
		UOSInt imgSize = img->info.dispSize.y * ((img->info.dispSize.x * img->info.storeBPP + 7) >> 3);
		UInt8 *imgData;
		imgData = MemAlloc(UInt8, imgSize);
		img->GetImageData(imgData, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (img->info.dispSize.x * img->info.storeBPP + 7) >> 3, false, Media::RotateType::None);
		switch (img->info.pf)
		{
		case Media::PF_B8G8R8A8:
		case Media::PF_LE_B16G16R16A16:
		case Media::PF_LE_FB32G32R32A32:
		case Media::PF_B8G8R8:
		case Media::PF_LE_B16G16R16:
		case Media::PF_LE_FB32G32R32:
			ImageUtil_SwapRGB(imgData, img->info.dispSize.x * img->info.dispSize.y, img->info.storeBPP);
			if (img->info.atype == Media::AT_NO_ALPHA)
			{
				UInt8 *tmpPtr = imgData;
				UOSInt cnt = img->info.dispSize.x * img->info.dispSize.y;
				if (img->info.pf == Media::PF_B8G8R8A8)
				{
					while (cnt-- > 0)
					{
						tmpPtr[3] = 0xff;
						tmpPtr += 4;
					}
				}
				else if (img->info.pf == Media::PF_LE_B16G16R16A16)
				{
					while (cnt-- > 0)
					{
						WriteInt16(&tmpPtr[6], 0xffff);
						tmpPtr += 8;
					}
				}
				else if (img->info.pf == Media::PF_LE_FB32G32R32A32)
				{
					while (cnt-- > 0)
					{
						WriteUInt32(&tmpPtr[12], 0xffffffff);
						tmpPtr += 16;
					}
				}
			}
			break;
		case Media::PF_UNKNOWN:
		case Media::PF_PAL_1:
		case Media::PF_PAL_2:
		case Media::PF_PAL_4:
		case Media::PF_PAL_8:
		case Media::PF_LE_R5G5B5:
		case Media::PF_LE_R5G6B5:
		case Media::PF_LE_W16:
		case Media::PF_LE_A2B10G10R10:
		case Media::PF_PAL_W1:
		case Media::PF_PAL_W2:
		case Media::PF_PAL_W4:
		case Media::PF_PAL_W8:
		case Media::PF_W8A8:
		case Media::PF_LE_W16A16:
		case Media::PF_LE_FW32A32:
		case Media::PF_LE_FW32:
		case Media::PF_R8G8B8A8:
		case Media::PF_R8G8B8:
		case Media::PF_PAL_1_A1:
		case Media::PF_PAL_2_A1:
		case Media::PF_PAL_4_A1:
		case Media::PF_PAL_8_A1:
		case Media::PF_B8G8R8A1:
		default:
			break;
		}
		stm->Write(imgData, imgSize);
		currOfst += imgSize;
		MemFree(imgData);

		WriteUInt32(&ifd[2 + l * 12], (UInt32)currOfst);
		stm->SeekFromBeginning(ifdOfst);
		stm->Write(ifd, l * 12 + 6);
		lastOfst = ifdOfst + l * 12 + 2;

		DEL_CLASS(newExif);
		MemFree(ifd);
	}
	stm->SeekFromBeginning(lastOfst);
	WriteInt32(buff, 0);
	stm->Write(buff, 4);
	return true;
}
