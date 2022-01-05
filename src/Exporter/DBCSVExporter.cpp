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
		Text::StrConcat(nameBuff, (const UTF8Char*)"DB CSV File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.csv");
		return true;
	}
	return false;
}

void Exporter::DBCSVExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBCSVExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	const UTF8Char *name = 0;
	if (param)
	{
		DBParam *dbParam = (DBParam*)param;
		name = dbParam->names->GetItem(dbParam->tableIndex);
	}
	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	DB::DBReader *r;
	r = db->GetTableData(name, 0, 0, 0, 0, 0);
	if (r == 0)
	{
		return false;
	}
	IO::BufferedOutputStream *cstm;
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt colCnt;
	UOSInt i;
	Bool firstCol;

	NEW_CLASS(cstm, IO::BufferedOutputStream(stm, 65536));
	NEW_CLASS(writer, IO::StreamWriter(cstm, this->codePage));
	writer->WriteSignature();

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
			sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
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
	writer->WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));

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
				sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
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
		writer->WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	db->CloseReader(r);
	DEL_CLASS(writer);
	DEL_CLASS(cstm);
	return true;
}

UOSInt Exporter::DBCSVExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBCSVExporter::CreateParam(IO::ParsedObject *pobj)
{
	DBParam *param = MemAlloc(DBParam, 1);
	param->db = (DB::ReadingDB *)pobj;
	NEW_CLASS(param->names, Data::ArrayListStrUTF8());
	param->db->GetTableNames(param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBCSVExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	DEL_CLASS(dbParam->names);
	MemFree(dbParam);
}

Bool Exporter::DBCSVExporter::GetParamInfo(UOSInt index, IO::FileExporter::ParamInfo *info)
{
	if (index == 0)
	{
		info->name = (const UTF8Char*)"Table";
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

Bool Exporter::DBCSVExporter::SetParamSel(void *param, UOSInt index, Int32 selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = (UOSInt)selCol;
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
		const UTF8Char *name = dbParam->names->GetItem(itemIndex);
		if (name)
		{
			return Text::StrConcat(buff, name);
		}
		return 0;
	}
	return 0;
}
