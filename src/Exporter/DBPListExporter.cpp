#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBExporter.h"
#include "DB/DBReader.h"
#include "Exporter/DBPListExporter.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/XML.h"

Exporter::DBPListExporter::DBPListExporter()
{
	this->codePage = 65001;
}

Exporter::DBPListExporter::~DBPListExporter()
{
}

Int32 Exporter::DBPListExporter::GetName()
{
	return *(Int32*)"DPLS";
}

IO::FileExporter::SupportType Exporter::DBPListExporter::IsObjectSupported(NotNullPtr<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBPListExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB Parameter List File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.plist"));
		return true;
	}
	return false;
}

void Exporter::DBPListExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBPListExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, NotNullPtr<IO::ParsedObject> pobj, void *param)
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
	return DB::DBExporter::GeneratePList(db, CSTR_NULL, Text::String::OrEmpty(name)->ToCString(), 0, stm, this->codePage);
}

UOSInt Exporter::DBPListExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBPListExporter::CreateParam(NotNullPtr<IO::ParsedObject> pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = NotNullPtr<DB::ReadingDB>::ConvertFrom(pobj);
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBPListExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	LIST_FREE_STRING(&dbParam->names);
	DEL_CLASS(dbParam);
}

Bool Exporter::DBPListExporter::GetParamInfo(UOSInt index, NotNullPtr<IO::FileExporter::ParamInfo> info)
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

Bool Exporter::DBPListExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamSel(void *param, UOSInt index, UOSInt selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::DBPListExporter::GetParamStr(void *param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamInt32(void *param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamSel(void *param, UOSInt index)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UTF8Char *Exporter::DBPListExporter::GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		Text::String *name = dbParam->names.GetItem(itemIndex);
		if (name)
		{
			return name->ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
