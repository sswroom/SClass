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
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CheckType::CRC32)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::SFVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("SFV File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.sfv"));
		return true;
	}
	return false;
}

Bool Exporter::SFVExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != IO::FileCheck::CheckType::CRC32)
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
		writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	DEL_CLASS(writer);
	return true;
}
