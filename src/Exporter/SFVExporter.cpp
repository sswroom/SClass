#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Exporter/SFVExporter.h"
#include "IO/FileCheck.h"
#include "Text/MyString.h"
#include "Text/UTF8Writer.h"

Exporter::SFVExporter::SFVExporter()
{
}

Exporter::SFVExporter::~SFVExporter()
{
}

Int32 Exporter::SFVExporter::GetName()
{
	return *(Int32*)"SFVE";
}

IO::FileExporter::SupportType Exporter::SFVExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CT_CRC32)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::SFVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"SFV File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.sfv");
		return true;
	}
	return false;
}

Bool Exporter::SFVExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_FILE_CHECK)
	{
		return false;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CT_CRC32)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UInt8 buff[16];
	Text::UTF8Writer *writer;
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	UOSInt i = 0;
	UOSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrConcat(sbuff, fchk->GetEntryName(i));
		*sptr++ = ' ';
		sptr = Text::StrHexVal32(sptr, ReadMUInt32(buff));
		writer->WriteLine(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	DEL_CLASS(writer);
	return true;
}
