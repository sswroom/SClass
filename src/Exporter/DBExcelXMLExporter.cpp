#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBReader.h"
#include "DB/DBUtil.h"
#include "Exporter/DBExcelXMLExporter.h"
#include "IO/StreamWriter.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"
#include "Text/XML.h"

Exporter::DBExcelXMLExporter::DBExcelXMLExporter()
{
	this->codePage = 65001;
}

Exporter::DBExcelXMLExporter::~DBExcelXMLExporter()
{
}

Int32 Exporter::DBExcelXMLExporter::GetName()
{
	return *(Int32*)"DEXM";
}

IO::FileExporter::SupportType Exporter::DBExcelXMLExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::DBExcelXMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"DB Excel XML File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.xml");
		return true;
	}
	return false;
}

void Exporter::DBExcelXMLExporter::SetCodePage(UInt32 codePage)
{
//	this->codePage = codePage;
}

Bool Exporter::DBExcelXMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	Text::Encoding *enc;
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt tableCnt;
	UOSInt colCnt;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;


	NEW_CLASS(enc, Text::Encoding(this->codePage));
	NEW_CLASS(writer, IO::StreamWriter(stm, enc));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	DB::DBReader *r;

	sptr = Text::StrConcat(Text::EncodingFactory::GetInternetName(Text::StrConcat(lineBuff1, (const UTF8Char*)"<?xml version=\"1.0\" encoding=\""), this->codePage), (const UTF8Char*)"\"?>");
	writer->WriteLine(lineBuff1);
	writer->WriteLine((const UTF8Char*)"<?mso-application progid=\"Excel.Sheet\"?>");
	writer->WriteLine((const UTF8Char*)"<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"");
	writer->WriteLine((const UTF8Char*)" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"");
	writer->WriteLine((const UTF8Char*)" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"");
	writer->WriteLine((const UTF8Char*)" xmlns:html=\"http://www.w3.org/TR/REC-html40\">");

	Data::ArrayListStrUTF8 *names;
	NEW_CLASS(names, Data::ArrayListStrUTF8());
	tableCnt = db->GetTableNames(names);
	j = 0;
	while (j < tableCnt)
	{
		const UTF8Char *tableName = names->GetItem(j);
		r = db->GetTableData(tableName, 0, 0, 0);
		if (r)
		{
			UOSInt ind = Text::StrLastIndexOf(tableName, '\\');
			sptr = Text::StrConcat(Text::XML::ToAttrText(Text::StrConcat(lineBuff1, (const UTF8Char*)" <Worksheet ss:Name="), &tableName[ind + 1]), (const UTF8Char*)">");
			Text::StrReplace(lineBuff1, '?', '_');
			Text::StrReplace(lineBuff1, '\\', '_');
			writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
			writer->WriteLine((const UTF8Char*)"  <Table>");

			writer->WriteLine((const UTF8Char*)"   <Row>");
			colCnt = r->ColCount();
			i = 0;
			while (i < colCnt)
			{
				if (r->GetName(i, lineBuff1))
				{
					sptr = Text::StrConcat(Text::XML::ToXMLText(Text::StrConcat(lineBuff2, (const UTF8Char*)"    <Cell><Data ss:Type=\"String\">"), lineBuff1), (const UTF8Char*)"</Data></Cell>");
					writer->WriteLine(lineBuff2, (UOSInt)(sptr - lineBuff2));
				}
				else
				{
					writer->WriteLine((const UTF8Char*)"    <Cell><Data ss:Type=\"String\"></Data></Cell>");
				}
				i++;
			}
			writer->WriteLine((const UTF8Char*)"   </Row>");

			while (r->ReadNext())
			{
				writer->WriteLine((const UTF8Char*)"   <Row>");
				colCnt = r->ColCount();
				i = 0;
				while (i < colCnt)
				{
					UOSInt colSize;
					DB::DBUtil::ColType ct = r->GetColType(i, &colSize);
					switch (ct)
					{
					case DB::DBUtil::CT_DateTime2:
					case DB::DBUtil::CT_DateTime:
						r->GetDate(i, &dt);
						sptr = Text::StrConcat(dt.ToString(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"DateTime\">"), "yyyy-MM-ddTHH:mm:ss.fff"), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Double:
					case DB::DBUtil::CT_Float:
						sptr = Text::StrConcat(Text::StrDouble(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"Number\">"), r->GetDbl(i)), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Byte:
					case DB::DBUtil::CT_Int16:
					case DB::DBUtil::CT_Int32:
					case DB::DBUtil::CT_UInt16:
						sptr = Text::StrConcat(Text::StrInt32(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"Number\">"), r->GetInt32(i)), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Int64:
					case DB::DBUtil::CT_UInt32:
						sptr = Text::StrConcat(Text::StrInt64(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"Number\">"), r->GetInt64(i)), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_UInt64:
						sptr = Text::StrConcat(Text::StrUInt64(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"Number\">"), (UInt64)r->GetInt64(i)), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_VarChar:
					case DB::DBUtil::CT_Char:
					case DB::DBUtil::CT_NVarChar:
					case DB::DBUtil::CT_NChar:
					case DB::DBUtil::CT_Vector:
					case DB::DBUtil::CT_Binary:
					case DB::DBUtil::CT_Bool:
					case DB::DBUtil::CT_Unknown:
					default:
						lineBuff2[0] = 0;
						r->GetStr(i, lineBuff2, 65536);
						sptr = Text::StrConcat(Text::XML::ToXMLText(Text::StrConcat(lineBuff1, (const UTF8Char*)"    <Cell><Data ss:Type=\"String\">"), lineBuff2), (const UTF8Char*)"</Data></Cell>");
						writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					}

					i++;
				}
				writer->WriteLine((const UTF8Char*)"   </Row>");
			}
			
			writer->WriteLine((const UTF8Char*)"  </Table>");
			writer->WriteLine((const UTF8Char*)" </Worksheet>");
			db->CloseReader(r);
		}
		j++;
	}
	writer->WriteLine((const UTF8Char*)"</Workbook>");

	DEL_CLASS(names);

	MemFree(lineBuff2);
	MemFree(lineBuff1);

	DEL_CLASS(enc);
	DEL_CLASS(writer);
	return true;
}
