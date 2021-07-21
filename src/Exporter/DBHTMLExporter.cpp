#include "Stdafx.h"
#include "MyMemory.h"
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

IO::FileExporter::SupportType Exporter::DBHTMLExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::DBHTMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"DB HTML File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.html");
		return true;
	}
	return false;
}

void Exporter::DBHTMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DBHTMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
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
	UOSInt colCnt;
	UOSInt i;
	OSInt si;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	writer->WriteLine((const UTF8Char*)"<html>");
	writer->WriteLine((const UTF8Char*)"<head>");
	writer->Write((const UTF8Char*)"<title>");
	db->GetSourceName(lineBuff1);
	sptr = lineBuff1;
	if ((si = Text::StrLastIndexOf(sptr, '\\')) >= 0)
		sptr = &sptr[si + 1];
	Text::XML::ToXMLText(lineBuff2, sptr);
	writer->WriteLine(lineBuff2);
	writer->WriteLine((const UTF8Char*)"</title>");
	sptr = Text::StrConcat(Text::EncodingFactory::GetInternetName(Text::StrConcat(lineBuff1, (const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset="), this->codePage), (const UTF8Char*)"\">");
	writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLine((const UTF8Char*)"</head>");
	db->GetSourceName(lineBuff2);
	sptr = lineBuff2;
	if ((si = Text::StrLastIndexOf(sptr, '\\')) >= 0)
		sptr = &sptr[si + 1];
	sptr = Text::XML::ToXMLText(Text::StrConcat(lineBuff1, (const UTF8Char*)"<body><h1>"), sptr);
	writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLine((const UTF8Char*)"</h1>");
	writer->WriteLine((const UTF8Char*)"<table border=1 cellspacing=1 cellpadding=0><tr>");

	sptr = lineBuff2;
	colCnt = r->ColCount();
	i = 0;
	while (i < colCnt)
	{
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"<th>");
		if (r->GetName(i, lineBuff1))
		{
			sptr = Text::XML::ToXMLText(sptr, lineBuff1);
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"</th>");
		i++;
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"</tr>");
	writer->WriteLine(lineBuff2, (UOSInt)(sptr - lineBuff2));

	while (r->ReadNext())
	{
		writer->WriteLine((const UTF8Char*)"<tr>");
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"<td>");
			if (r->GetStr(i, lineBuff1, 65536))
			{
				sptr = Text::XML::ToXMLText(sptr, lineBuff1);
			}
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"</td>");
			i++;
		}
		writer->WriteLine(lineBuff2, (UOSInt)(sptr - lineBuff2));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer->WriteLine((const UTF8Char*)"</table>");
	writer->WriteLine((const UTF8Char*)"</body></html>");

	db->CloseReader(r);
	DEL_CLASS(writer);
	return true;
}

UOSInt Exporter::DBHTMLExporter::GetParamCnt()
{
	return 1;
}

void *Exporter::DBHTMLExporter::CreateParam(IO::ParsedObject *pobj)
{
	DBParam *param = MemAlloc(DBParam, 1);
	param->db = (DB::ReadingDB *)pobj;
	NEW_CLASS(param->names, Data::ArrayListStrUTF8());
	param->db->GetTableNames(param->names);
	param->tableIndex = 0;
	return param;
}

void Exporter::DBHTMLExporter::DeleteParam(void *param)
{
	DBParam *dbParam = (DBParam*)param;
	DEL_CLASS(dbParam->names);
	MemFree(dbParam);
}

Bool Exporter::DBHTMLExporter::GetParamInfo(UOSInt index, IO::FileExporter::ParamInfo *info)
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

Bool Exporter::DBHTMLExporter::SetParamStr(void *param, UOSInt index, const UTF8Char *val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamInt32(void *param, UOSInt index, Int32 val)
{
	return false;
}

Bool Exporter::DBHTMLExporter::SetParamSel(void *param, UOSInt index, Int32 selCol)
{
	if (index == 0)
	{
		DBParam *dbParam = (DBParam*)param;
		dbParam->tableIndex = (UOSInt)selCol;
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
		const UTF8Char *name = dbParam->names->GetItem(itemIndex);
		if (name)
		{
			return Text::StrConcat(buff, name);
		}
		return 0;
	}
	return 0;
}
