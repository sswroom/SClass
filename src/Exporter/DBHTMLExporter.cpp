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

IO::FileExporter::SupportType Exporter::DBHTMLExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBHTMLExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
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

Bool Exporter::DBHTMLExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Optional<Text::String> name = 0;
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		name = dbParam->names.GetItem(dbParam->tableIndex);
	}
	NN<DB::ReadingDB> db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	return DB::DBExporter::GenerateHTML(db, CSTR_NULL, Text::String::OrEmpty(name)->ToCString(), 0, stm, this->codePage);
}

UOSInt Exporter::DBHTMLExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::DBHTMLExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return (ParamData*)param;
}

void Exporter::DBHTMLExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		dbParam->names.FreeAll();
		DEL_CLASS(dbParam);
	}
}

Bool Exporter::DBHTMLExporter::GetParamInfo(UOSInt index, NN<IO::FileExporter::ParamInfo> info)
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

Bool Exporter::DBHTMLExporter::SetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArrayOpt<const UTF8Char> val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamSel(Optional<ParamData> param, UOSInt index, UOSInt selCol)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		dbParam->tableIndex = selCol;
		return true;
	}
	return false;
}

UnsafeArrayOpt<UTF8Char> Exporter::DBHTMLExporter::GetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Int32 Exporter::DBHTMLExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBHTMLExporter::GetParamSel(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Exporter::DBHTMLExporter::GetParamSelItems(Optional<ParamData> param, UOSInt index, UOSInt itemIndex, UnsafeArray<UTF8Char> buff)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		NN<Text::String> name;
		if (dbParam->names.GetItem(itemIndex).SetTo(name) && name->leng > 0)
		{
			return name->ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
