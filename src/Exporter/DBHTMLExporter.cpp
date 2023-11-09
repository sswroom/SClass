#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBExporter.h"
#include "DB/DBReader.h"
#include "Exporter/DBHTMLExporter.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/XML.h"

Exporter::DBHTMLExporter::DBHTMLExporter()
{
	this->codePage = 65001;
}

Exporter::DBHTMLExporter::~DBHTMLExporter()
{
}

Int32 Exporter::DBHTMLExporter::GetName()
{
	return *(Int32*)"DHTM";
}

IO::FileExporter::SupportType Exporter::DBHTMLExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBHTMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB HTML File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.html"));
		return true;
	}
	return false;
}

void Exporter::DBHTMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBHTMLExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Text::String *name = 0;
	if (param)
	{
		DBParam *dbParam = (DBParam*)param;
		name = dbParam->names.GetItem(dbParam->tableIndex);
	}
	NotNullPtr<DB::ReadingDB> db = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	return DB::DBExporter::GenerateHTML(db, CSTR_NULL, Text::String::OrEmpty(name)->ToCString(), 0, stm, this->codePage);
}

UOSInt Exporter::DBHTMLExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBHTMLExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBHTMLExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	LIST_FREE_STRING(&dbParam->names);
	DEL_CLASS(dbParam);
}

Bool Exporter::DBHTMLExporter::GetParamInfo(UOSInt index, NotNullPtr<IO::FileExporter::ParamInfo> info)
{
	if (index == 0)
	{
		info->name = CSTR("Table");
		info->paramType = IO::FileExporter::ParamType::SELECTION;
		info->allowNull = true;
		return true;
	}
	else
	{
		return false;
	}
}

Bool Exporter::DBHTMLExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamSel(void *param, UOSInt index, UOSInt selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::DBHTMLExporter::GetParamStr(void *param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::DBHTMLExporter::GetParamInt32(void *param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBHTMLExporter::GetParamSel(void *param, UOSInt index)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UTF8Char *Exporter::DBHTMLExporter::GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		Text::String *name = dbParam->names.GetItem(itemIndex);
		if (name->leng > 0)
		{
			return name->ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
