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

IO::FileExporter::SupportType Exporter::DBPListExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBPListExporter::GetOutputName(UOSInt index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
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

Bool Exporter::DBPListExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Optional<Text::String> name = nullptr;
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		name = dbParam->names.GetItem(dbParam->tableIndex);
	}
	NN<DB::ReadingDB> db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	return DB::DBExporter::GeneratePList(db, nullptr, Text::String::OrEmpty(name)->ToCString(), nullptr, stm, this->codePage);
}

UOSInt Exporter::DBPListExporter::GetParamCnt()
{
	return 1;
}

Optional<IO::FileExporter::ParamData> Exporter::DBPListExporter::CreateParam(NN<IO::ParsedObject> pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = NN<DB::ReadingDB>::ConvertFrom(pobj);
	param->db->QueryTableNames(nullptr, param->names);
	param->tableIndex = 0;
	return (ParamData*)param;
}

void Exporter::DBPListExporter::DeleteParam(Optional<ParamData> param)
{
	NN<ParamData> para;
	if (param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		dbParam->names.FreeAll();
		DEL_CLASS(dbParam);
	}
}

Bool Exporter::DBPListExporter::GetParamInfo(UOSInt index, NN<IO::FileExporter::ParamInfo> info)
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

Bool Exporter::DBPListExporter::SetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArrayOpt<const UTF8Char> val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamInt32(Optional<ParamData> param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamSel(Optional<ParamData> param, UOSInt index, UOSInt selCol)
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

UnsafeArrayOpt<UTF8Char> Exporter::DBPListExporter::GetParamStr(Optional<ParamData> param, UOSInt index, UnsafeArray<UTF8Char> buff)
{
	return nullptr;
}

Int32 Exporter::DBPListExporter::GetParamInt32(Optional<ParamData> param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamSel(Optional<ParamData> param, UOSInt index)
{
	NN<ParamData> para;
	if (index == 0 && param.SetTo(para))
	{
		DBParam *dbParam = (DBParam*)para.Ptr();
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Exporter::DBPListExporter::GetParamSelItems(Optional<ParamData> param, UOSInt index, UOSInt itemIndex, UnsafeArray<UTF8Char> buff)
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
		return nullptr;
	}
	return nullptr;
}
