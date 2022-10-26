#include "Stdafx.h"
#include "Exporter/ZIPExporter.h"
#include "IO/PackageFile.h"

Exporter::ZIPExporter::ZIPExporter()
{
}

Exporter::ZIPExporter::~ZIPExporter()
{
}

Int32 Exporter::ZIPExporter::GetName()
{
	return *(Int32*)"ZIPE";
}

IO::FileExporter::SupportType Exporter::ZIPExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::ZIPExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("ZIP File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.zip"));
		return true;
	}
	return false;
}

Bool Exporter::ZIPExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
	{
		return false;
	}
	UTF8Char sbuff[512];
	IO::ZIPBuilder *zip;
	NEW_CLASS(zip, IO::ZIPBuilder(stm));
	Bool succ = this->ExportPackage(zip, sbuff, sbuff, (IO::PackageFile*)pobj);
	DEL_CLASS(zip);
	return succ;
}

Bool Exporter::ZIPExporter::ExportPackage(IO::ZIPBuilder *zip, UTF8Char *buffStart, UTF8Char *buffEnd, IO::PackageFile *pkg)
{
	UOSInt i;
	UOSInt j;
	UTF8Char *sptr;
	IO::PackageFile::PackObjectType itemType;
	IO::IStreamData *fd;
	UInt8 *fileBuff;
	UInt64 fileLeng;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		itemType = pkg->GetItemType(i);
		sptr = pkg->GetItemName(buffEnd, i);
		if (itemType == IO::PackageFile::PackObjectType::StreamData)
		{
			fd = pkg->GetItemStmData(i);
			fileLeng = fd->GetDataSize();
			fileBuff = MemAlloc(UInt8, (UOSInt)fileLeng);
			if (fd->GetRealData(0, (UOSInt)fileLeng, fileBuff) != fileLeng)
			{
				MemFree(fileBuff);
				return false;
			}
			zip->AddFile(CSTRP(buffStart, sptr), fileBuff, (UOSInt)fileLeng, pkg->GetItemModTime(i).ToTicks(), false);
			MemFree(fileBuff);
		}
		else if (itemType == IO::PackageFile::PackObjectType::PackageFile)
		{
			*sptr++ = '/';
			if (!this->ExportPackage(zip, buffStart, sptr, pkg->GetItemPack(i)))
			{
				return false;
			}
		}
		i++;
	}
	return true;
}
