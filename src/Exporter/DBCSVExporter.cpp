#include "Stdafx.h"
#include "MyMemory.h"
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

IO::FileExporter::SupportType Exporter::DBCSVExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBCSVExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
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

Bool Exporter::DBCSVExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
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
	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	NotNullPtr<DB::DBReader> r;
	if (!r.Set(db->QueryTableData(CSTR_NULL, STR_CSTR(name), 0, 0, 0, CSTR_NULL, 0)))
	{
		return false;
	}
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt colCnt;
	UOSInt i;
	Bool firstCol;

	IO::BufferedOutputStream cstm(stm, 65536);
	IO::StreamWriter writer(cstm, this->codePage);
	writer.WriteSignature();

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	sptr = lineBuff2;
	colCnt = r->ColCount();
	i = 0;
	firstCol = true;
	while (i < colCnt)
	{
		if (firstCol)
		{
			firstCol = false;
		}
		else
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		}

		if (r->GetName(i, lineBuff1))
		{
			sptr = Text::StrToCSVRec(sptr, lineBuff1);
		}
		else
		{
			sptr = Text::StrToCSVRec(sptr, 0);
		}
		i++;
	}
	writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));

	while (r->ReadNext())
	{
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		firstCol = true;
		while (i < colCnt)
		{
			if (firstCol)
			{
				firstCol = false;
			}
			else
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			}

			if (r->GetStr(i, lineBuff1, 65536))
			{
				sptr = Text::StrToCSVRec(sptr, lineBuff1);
			}
			else
			{
				sptr = Text::StrToCSVRec(sptr, (const UTF8Char*)"");
			}
			i++;
		}
		writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	db->CloseReader(r);
	return true;
}

UOSInt Exporter::DBCSVExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBCSVExporter::CreateParam(IO::ParsedObject *pobj)
{
	DBParam *param;
	NEW_CLASS(param, DBParam());
	param->db = (DB::ReadingDB *)pobj;
	param->db->QueryTableNames(CSTR_NULL, param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBCSVExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	LIST_FREE_STRING(&dbParam->names);
	DEL_CLASS(dbParam);
}

Bool Exporter::DBCSVExporter::GetParamInfo(UOSInt index, IO::FileExporter::ParamInfo *info)
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

Bool Exporter::DBCSVExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBCSVExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBCSVExporter::SetParamSel(void *param, UOSInt index, UOSInt selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::DBCSVExporter::GetParamStr(void *param, UOSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::DBCSVExporter::GetParamInt32(void *param, UOSInt index)
{
	return 0;
}

Int32 Exporter::DBCSVExporter::GetParamSel(void *param, UOSInt index)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UTF8Char *Exporter::DBCSVExporter::GetParamSelItems(void *param, UOSInt index, UOSInt itemIndex, UTF8Char *buff)
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
