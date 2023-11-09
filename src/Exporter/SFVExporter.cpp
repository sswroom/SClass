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

IO::FileExporter::SupportType Exporter::SFVExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NotNullPtr<IO::FileCheck> fchk = NotNullPtr<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::CRC32)
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

Bool Exporter::SFVExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	NotNullPtr<IO::FileCheck> fchk = NotNullPtr<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::CRC32)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UInt8 buff[16];
	Text::UTF8Writer writer(stm);
	UOSInt i = 0;
	UOSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = fchk->GetEntryName(i)->ConcatTo(sbuff);
		*sptr++ = ' ';
		sptr = Text::StrHexVal32(sptr, ReadMUInt32(buff));
		writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	return true;
}
