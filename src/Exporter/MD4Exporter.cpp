#include "Stdafx.h"
#include "MyMemory.h"
#include "Exporter/MD4Exporter.h"
#include "Text/MyString.h"
#include "IO/FileCheck.h"
#include "IO/StreamWriter.h"

Exporter::MD4Exporter::MD4Exporter()
{
	this->codePage = 65001;
}

Exporter::MD4Exporter::~MD4Exporter()
{
}

Int32 Exporter::MD4Exporter::GetName()
{
	return *(Int32*)"MD4E";
}

IO::FileExporter::SupportType Exporter::MD4Exporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD4)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::MD4Exporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("MD4 File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.md4"));
		return true;
	}
	return false;
}

void Exporter::MD4Exporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::MD4Exporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD4)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[16];
	Text::Encoding enc(this->codePage);
	IO::StreamWriter writer(stm, enc);
	UOSInt i = 0;
	UOSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" *"));
		sptr = Text::String::OrEmpty(fchk->GetEntryName(i))->ConcatTo(sptr);
		writer.WriteLine(CSTRP(sbuff, sptr));
		i++;
	}
	return true;
}
