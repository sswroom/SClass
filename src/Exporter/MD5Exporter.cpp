#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/MD5Exporter.h"
#include "IO/FileCheck.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"

Exporter::MD5Exporter::MD5Exporter()
{
	this->codePage = 65001;
}

Exporter::MD5Exporter::~MD5Exporter()
{
}

Int32 Exporter::MD5Exporter::GetName()
{
	return *(Int32*)"MD5E";
}

IO::FileExporter::SupportType Exporter::MD5Exporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CT_MD5)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::MD5Exporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"MD5 File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.md5");
		return true;
	}
	return false;
}

void Exporter::MD5Exporter::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::MD5Exporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return false;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CT_MD5)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UInt8 buff[16];
	IO::StreamWriter *writer;
	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));
	OSInt i = 0;
	OSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)" *");
		sptr2 = sptr;
		sptr = Text::StrConcat(sptr, fchk->GetEntryName(i));
		Text::StrReplace(sptr2, '/', '\\');
		writer->WriteLine(sbuff, sptr - sbuff);
		i++;
	}
	DEL_CLASS(writer);
	return true;
}
