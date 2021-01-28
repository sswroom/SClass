#include "Stdafx.h"
#include "MyMemory.h"
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

IO::FileExporter::SupportType Exporter::DBPListExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::DBPListExporter::GetOutputName(OSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"DB Parameter List File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.plist");
		return true;
	}
	return false;
}

void Exporter::DBPListExporter::SetCodePage(Int32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBPListExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
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
	r = db->GetTableData(name, 0, 0, 0);
	if (r == 0)
	{
		return false;
	}
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	OSInt colCnt;
	OSInt i;
	UOSInt colSize;
	Data::DateTime dt;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	sptr = Text::StrConcat(Text::EncodingFactory::GetInternetName(Text::StrConcat(lineBuff1, (const UTF8Char*)"<?xml version=\"1.0\" encoding=\""), this->codePage), (const UTF8Char*)"\"?>");
	writer->WriteLine(lineBuff1);
	writer->WriteLine((const UTF8Char*)"<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">");
	writer->WriteLine((const UTF8Char*)"<plist version=\"1.0\">");
	writer->WriteLine((const UTF8Char*)"<array>");

	sptr = lineBuff2;
	colCnt = r->ColCount();
	while (r->ReadNext())
	{
		writer->WriteLine((const UTF8Char*)"    <dict>");
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			r->GetName(i, lineBuff1);
			Text::StrConcat(Text::XML::ToXMLText(Text::StrConcat(lineBuff2, (const UTF8Char*)"        <key>"), lineBuff1), (const UTF8Char*)"</key>");
			writer->WriteLine(lineBuff2);

			DB::DBUtil::ColType ct = r->GetColType(i, &colSize);
			switch (ct)
			{
			case DB::DBUtil::CT_DateTime:
				r->GetDate(i, &dt);
				sptr = Text::StrConcat(dt.ToString(Text::StrConcat(lineBuff1, (const UTF8Char*)"        <string>"), "yyyy-MM-dd HH:mm:ss"), (const UTF8Char*)"</string>");
				writer->WriteLine(lineBuff1, sptr - lineBuff1);
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				sptr = Text::StrConcat(Text::StrDouble(Text::StrConcat(lineBuff1, (const UTF8Char*)"        <string>"), r->GetDbl(i)), (const UTF8Char*)"</string>");
				writer->WriteLine(lineBuff1, sptr - lineBuff1);
				break;
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_UInt16:
				sptr = Text::StrConcat(Text::StrInt32(Text::StrConcat(lineBuff1, (const UTF8Char*)"        <integer>"), r->GetInt32(i)), (const UTF8Char*)"</integer>");
				writer->WriteLine(lineBuff1, sptr - lineBuff1);
				break;
			case DB::DBUtil::CT_Int64:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt64:
				sptr = Text::StrConcat(Text::StrInt64(Text::StrConcat(lineBuff1, (const UTF8Char*)"        <integer>"), r->GetInt64(i)), (const UTF8Char*)"</integer>");
				writer->WriteLine(lineBuff1, sptr - lineBuff1);
				break;
			default:
				if (r->GetStr(i, lineBuff2, 65536))
				{
					sptr = Text::StrConcat(Text::XML::ToXMLText(Text::StrConcat(lineBuff1, (const UTF8Char*)"        <string>"), lineBuff2), (const UTF8Char*)"</string>");
				}
				else
				{
					sptr = Text::StrConcat(lineBuff1, (const UTF8Char*)"        <string>(null)</string>");
				}
				writer->WriteLine(lineBuff1, sptr - lineBuff1);
				break;
			}

			i++;
		}
		writer->WriteLine((const UTF8Char*)"    </dict>");
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer->WriteLine((const UTF8Char*)"</array>");
	writer->WriteLine((const UTF8Char*)"</plist>");

	db->CloseReader(r);
	DEL_CLASS(writer);
	return true;
}

OSInt Exporter::DBPListExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBPListExporter::CreateParam(IO::ParsedObject *pobj)
{
	DBParam *param = MemAlloc(DBParam, 1);
	param->db = (DB::ReadingDB *)pobj;
	NEW_CLASS(param->names, Data::ArrayListStrUTF8());
	param->db->GetTableNames(param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBPListExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	DEL_CLASS(dbParam->names);
	MemFree(dbParam);
}

Bool Exporter::DBPListExporter::GetParamInfo(OSInt index, IO::FileExporter::ParamInfo *info)
{
	if (index == 0)
	{
		info->name = (const UTF8Char*)"Table";
		info->paramType = IO::FileExporter::PT_SELECTION;
		info->allowNull = true;
		return true;
	}
	else
	{
		return false;
	}
}

Bool Exporter::DBPListExporter::SetParamStr(void *param, OSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamInt32(void *param, OSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBPListExporter::SetParamSel(void *param, OSInt index, Int32 selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = selCol;
		return true;
	}
	return false;
}

UTF8Char *Exporter::DBPListExporter::GetParamStr(void *param, OSInt index, UTF8Char *buff)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamInt32(void *param, OSInt index)
{
	return 0;
}

Int32 Exporter::DBPListExporter::GetParamSel(void *param, OSInt index)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		return (Int32)dbParam->tableIndex;
	}
	return 0;
}

UTF8Char *Exporter::DBPListExporter::GetParamSelItems(void *param, OSInt index, OSInt itemIndex, UTF8Char *buff)
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
