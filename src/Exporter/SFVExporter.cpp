#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
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

IO::FileExporter::SupportType Exporter::SFVExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::CRC32)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::SFVExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("SFV File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.sfv"));
		return true;
	}
	return false;
}

Bool Exporter::SFVExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::CRC32)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[16];
	Text::UTF8Writer writer(stm);
	UIntOS i = 0;
	UIntOS cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::String::OrEmpty(fchk->GetEntryName(i))->ConcatTo(sbuff);
		*sptr++ = ' ';
		sptr = Text::StrHexVal32(sptr, ReadMUInt32(buff));
		writer.WriteLine(CSTRP(sbuff, sptr));
		i++;
	}
	return true;
}
