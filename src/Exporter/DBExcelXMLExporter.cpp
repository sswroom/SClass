#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBExporter.h"
#include "DB/DBReader.h"
#include "DB/DBUtil.h"
#include "Exporter/DBExcelXMLExporter.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/XML.h"

Exporter::DBExcelXMLExporter::DBExcelXMLExporter()
{
	this->codePage = 65001;
}

Exporter::DBExcelXMLExporter::~DBExcelXMLExporter()
{
}

Int32 Exporter::DBExcelXMLExporter::GetName()
{
	return *(Int32*)"DEXM";
}

IO::FileExporter::SupportType Exporter::DBExcelXMLExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBExcelXMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB Excel XML File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.xml"));
		return true;
	}
	return false;
}

void Exporter::DBExcelXMLExporter::SetCodePage(UInt32 codePage)
{
//	this->codePage = codePage;
}

Bool Exporter::DBExcelXMLExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	NotNullPtr<DB::ReadingDB> db = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	return DB::DBExporter::GenerateExcelXMLAllTables(db, CSTR_NULL, stm, this->codePage);
}
