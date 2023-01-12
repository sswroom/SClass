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
	IO::ZIPMTBuilder zip(stm);
	return this->ExportPackage(&zip, sbuff, sbuff, (IO::PackageFile*)pobj);
}

Bool Exporter::ZIPExporter::ExportPackage(IO::ZIPMTBuilder *zip, UTF8Char *buffStart, UTF8Char *buffEnd, IO::PackageFile *pkg)
{
	UOSInt i;
	UOSInt j;
	UTF8Char *sptr;
	IO::PackageFile::PackObjectType itemType;
	IO::IStreamData *fd;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		itemType = pkg->GetItemType(i);
		sptr = pkg->GetItemName(buffEnd, i);
		if (itemType == IO::PackageFile::PackObjectType::StreamData)
		{
			fd = pkg->GetItemStmDataNew(i);
			if (!zip->AddFile(CSTRP(buffStart, sptr), fd, pkg->GetItemModTime(i).ToTicks(), Data::Compress::Inflate::CompressionLevel::BestCompression))
			{
				DEL_CLASS(fd);
				return false;
			}
			DEL_CLASS(fd);
		}
		else if (itemType == IO::PackageFile::PackObjectType::PackageFileType)
		{
			*sptr++ = '/';
			IO::PackageFile *innerPkg = pkg->GetItemPackNew(i);
			if (!this->ExportPackage(zip, buffStart, sptr, innerPkg))
			{
				DEL_CLASS(innerPkg);
				return false;
			}
			DEL_CLASS(innerPkg);
		}
		i++;
	}
	return true;
}
