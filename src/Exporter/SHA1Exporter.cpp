#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/SHA1Exporter.h"
#include "Text/MyString.h"
#include "IO/FileCheck.h"
#include "IO/StreamWriter.h"

Exporter::SHA1Exporter::SHA1Exporter()
{
	this->codePage = 65001;
}

Exporter::SHA1Exporter::~SHA1Exporter()
{
}

Int32 Exporter::SHA1Exporter::GetName()
{
	return *(Int32*)"SHA1";
}

IO::FileExporter::SupportType Exporter::SHA1Exporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CheckType::SHA1)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::SHA1Exporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"SHA-1 File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.sha1");
		return true;
	}
	return false;
}

void Exporter::SHA1Exporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::SHA1Exporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return false;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CheckType::SHA1)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UInt8 buff[20];
	IO::StreamWriter *writer;
	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));
	UOSInt i = 0;
	UOSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrHexBytes(sbuff, buff, 20, 0);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)" *");
		sptr = Text::StrConcat(sptr, fchk->GetEntryName(i));
		writer->WriteLine(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	DEL_CLASS(writer);
	return true;
}
