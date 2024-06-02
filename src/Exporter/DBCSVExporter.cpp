#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBExporter.h"
#include "DB/DBReader.h"
#include "Exporter/DBCSVExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"

Exporter::DBCSVExporter::DBCSVExporter()
{
	this->codePage = 65001;
}

Exporter::DBCSVExporter::~DBCSVExporter()
{
}

Int32 Exporter::DBCSVExporter::GetName()
{
	return *(Int32*)"DCSV";
}

IO::FileExporter::SupportType Exporter::DBCSVExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBCSVExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB CSV File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.csv"));
		return true;
	}
	return false;
}

void Exporter::DBCSVExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBCSVExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
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
	return DB::DBExporter::GenerateCSV(db, CSTR_NULL, Text::String::OrEmpty(name)->ToCString(), 0, CSTR("\"\""), stm, this->codePage);
}

UOSInt Exporter::DBCSVExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::DBCSVExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return (ParamData*)param;
}

void Exporter::DBCSVExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		dbParam->names.FreeAll();
		DEL_CLASS(dbParam);
	}
}

Bool Exporter::DBCSVExporter::GetParamInfo(UOSInt index, NN<IO::FileExporter::ParamInfo> info)
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

Bool Exporter::DBCSVExporter::SetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArrayOpt<const UTF8Char> val)
{
	return false;
}

Bool Exporter::DBCSVExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBCSVExporter::SetParamSel(Optional<ParamData> param, UOSInt index, UOSInt selCol)
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

UnsafeArrayOpt<UTF8Char> Exporter::DBCSVExporter::GetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArray<UTF8Char> buff)
{
	return 0;
}

Int32 Exporter::DBCSVExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBCSVExporter::GetParamSel(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Exporter::DBCSVExporter::GetParamSelItems(Optional<ParamData> param, UOSInt index, UOSInt itemIndex, UnsafeArray<UTF8Char> buff)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		NN<Text::String> name;
		if (dbParam->names.GetItem(itemIndex).SetTo(name))
		{
			return name->ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
