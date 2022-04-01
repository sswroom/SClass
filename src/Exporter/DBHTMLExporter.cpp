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

Bool Exporter::DBHTMLExporter::ExportFile(IO::SeekableStream *stm, Text::CString fileName, IO::ParsedObject *pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Text::CString name = CSTR_NULL;
	if (param)
	{
		DBParam *dbParam = (DBParam*)param;
		name = dbParam->names->GetItem(dbParam->tableIndex);
	}
	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	DB::DBReader *r;
	r = db->QueryTableData(name, 0, 0, 0, CSTR_NULL, 0);
	if (r == 0)
	{
		return false;
	}
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt colCnt;
	UOSInt i;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	writer->WriteLineC(UTF8STRC("<html>"));
	writer->WriteLineC(UTF8STRC("<head>"));
	writer->WriteStrC(UTF8STRC("<title>"));
	sptr2 = db->GetSourceName(lineBuff1);
	sptr = lineBuff1;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr2 = Text::XML::ToXMLText(lineBuff2, sptr);
	writer->WriteLineC(lineBuff2, (UOSInt)(sptr2 - lineBuff2));
	writer->WriteLineC(UTF8STRC("</title>"));
	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=")), this->codePage), UTF8STRC("\">"));
	writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLineC(UTF8STRC("</head>"));
	sptr2 = db->GetSourceName(lineBuff2);
	sptr = lineBuff2;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr = Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("<body><h1>")), sptr);
	writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLineC(UTF8STRC("</h1>"));
	writer->WriteLineC(UTF8STRC("<table border=1 cellspacing=1 cellpadding=0><tr>"));

	sptr = lineBuff2;
	colCnt = r->ColCount();
	i = 0;
	while (i < colCnt)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("<th>"));
		if (r->GetName(i, lineBuff1))
		{
			sptr = Text::XML::ToXMLText(sptr, lineBuff1);
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("</th>"));
		i++;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("</tr>"));
	writer->WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));

	while (r->ReadNext())
	{
		writer->WriteLineC(UTF8STRC("<tr>"));
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("<td>"));
			if (r->GetStr(i, lineBuff1, 65536))
			{
				sptr = Text::XML::ToXMLText(sptr, lineBuff1);
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("</td>"));
			i++;
		}
		writer->WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer->WriteLineC(UTF8STRC("</table>"));
	writer->WriteLineC(UTF8STRC("</body></html>"));

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
	NEW_CLASS(param->names, Data::ArrayList<Text::CString>());
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
		Text::CString name = dbParam->names->GetItem(itemIndex);
		if (name.v)
		{
			return name.ConcatTo(buff);
		}
		return 0;
	}
	return 0;
}
