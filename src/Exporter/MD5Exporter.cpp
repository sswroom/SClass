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

IO::FileExporter::SupportType Exporter::MD5Exporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD5)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::MD5Exporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("MD5 File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.md5"));
		return true;
	}
	return false;
}

void Exporter::MD5Exporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::MD5Exporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	NN<IO::FileCheck> fchk = NN<IO::FileCheck>::ConvertFrom(pobj);
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD5)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UInt8 buff[16];
	IO::StreamWriter writer(stm, this->codePage);
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" *"));
		sptr2 = sptr;
		if (fchk->GetEntryName(i).SetTo(s))
			sptr = s->ConcatTo(sptr);
		Text::StrReplace(sptr2, '/', '\\');
		writer.WriteLine(CSTRP(sbuff, sptr));
		i++;
	}
	return true;
}
