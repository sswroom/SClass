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

IO::FileExporter::SupportType Exporter::MD4Exporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD4)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::MD4Exporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
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

Bool Exporter::MD4Exporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::FileCheck)
	{
		return false;
	}
	IO::FileCheck *fchk = (IO::FileCheck *)pobj;
	if (fchk->GetCheckType() != Crypto::Hash::HashType::MD4)
	{
		return false;
	}

	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UInt8 buff[16];
	Text::Encoding enc(this->codePage);
	IO::StreamWriter writer(stm, &enc);
	UOSInt i = 0;
	UOSInt cnt = fchk->GetCount();
	while (i < cnt)
	{
		fchk->GetEntryHash(i, buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" *"));
		sptr = fchk->GetEntryName(i)->ConcatTo(sptr);
		writer.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		i++;
	}
	return true;
}
