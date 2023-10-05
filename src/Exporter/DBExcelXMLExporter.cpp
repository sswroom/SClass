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
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DBExcelXMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("DB Excel XML File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.xml"));
		return true;
	}
	return false;
}

void Exporter::DBExcelXMLExporter::SetCodePage(UInt32 codePage)
{
//	this->codePage = codePage;
}

Bool Exporter::DBExcelXMLExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
	if (!DB::ReadingDB::IsDBObj(pobj))
	{
		return false;
	}

	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt tableCnt;
	UOSInt colCnt;
	UOSInt i;
	UOSInt j;


	Text::Encoding enc(this->codePage);
	IO::StreamWriter writer(stm, &enc);

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	DB::ReadingDB *db = (DB::ReadingDB*)pobj;
	NotNullPtr<DB::DBReader> r;

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), this->codePage), UTF8STRC("\"?>"));
	writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer.WriteLineC(UTF8STRC("<?mso-application progid=\"Excel.Sheet\"?>"));
	writer.WriteLineC(UTF8STRC("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLineC(UTF8STRC(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\""));
	writer.WriteLineC(UTF8STRC(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLineC(UTF8STRC(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">"));

	Data::ArrayListNN<Text::String> names;
	tableCnt = db->QueryTableNames(CSTR_NULL, names);
	j = 0;
	while (j < tableCnt)
	{
		Text::String *tableName = names.GetItem(j);
		if (r.Set(db->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0)))
		{
			UOSInt ind = tableName->LastIndexOf('\\');
			sptr = Text::StrConcatC(Text::XML::ToAttrText(Text::StrConcatC(lineBuff1, UTF8STRC(" <Worksheet ss:Name=")), &tableName->v[ind + 1]), UTF8STRC(">"));
			Text::StrReplace(lineBuff1, '?', '_');
			Text::StrReplace(lineBuff1, '\\', '_');
			writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
			writer.WriteLineC(UTF8STRC("  <Table>"));

			writer.WriteLineC(UTF8STRC("   <Row>"));
			colCnt = r->ColCount();
			i = 0;
			while (i < colCnt)
			{
				if (r->GetName(i, lineBuff1))
				{
					sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff2, UTF8STRC("    <Cell><Data ss:Type=\"String\">")), lineBuff1), UTF8STRC("</Data></Cell>"));
					writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));
				}
				else
				{
					writer.WriteLineC(UTF8STRC("    <Cell><Data ss:Type=\"String\"></Data></Cell>"));
				}
				i++;
			}
			writer.WriteLineC(UTF8STRC("   </Row>"));

			while (r->ReadNext())
			{
				writer.WriteLineC(UTF8STRC("   <Row>"));
				colCnt = r->ColCount();
				i = 0;
				while (i < colCnt)
				{
					UOSInt colSize;
					DB::DBUtil::ColType ct = r->GetColType(i, colSize);
					switch (ct)
					{
					case DB::DBUtil::CT_Date:
					case DB::DBUtil::CT_DateTime:
					case DB::DBUtil::CT_DateTimeTZ:
						sptr = Text::StrConcatC(r->GetTimestamp(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"DateTime\">")), "yyyy-MM-ddTHH:mm:ss.fff"), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Double:
					case DB::DBUtil::CT_Float:
					case DB::DBUtil::CT_Decimal:
						sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetDbl(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Byte:
					case DB::DBUtil::CT_Int16:
					case DB::DBUtil::CT_Int32:
					case DB::DBUtil::CT_UInt16:
						sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetInt32(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_Int64:
					case DB::DBUtil::CT_UInt32:
						sptr = Text::StrConcatC(Text::StrInt64(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetInt64(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_UInt64:
						sptr = Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), (UInt64)r->GetInt64(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					case DB::DBUtil::CT_UTF8Char:
					case DB::DBUtil::CT_UTF16Char:
					case DB::DBUtil::CT_UTF32Char:
					case DB::DBUtil::CT_VarUTF8Char:
					case DB::DBUtil::CT_VarUTF16Char:
					case DB::DBUtil::CT_VarUTF32Char:
					case DB::DBUtil::CT_Vector:
					case DB::DBUtil::CT_Binary:
					case DB::DBUtil::CT_Bool:
					case DB::DBUtil::CT_UUID:
					case DB::DBUtil::CT_Unknown:
					default:
						lineBuff2[0] = 0;
						r->GetStr(i, lineBuff2, 65536);
						sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"String\">")), lineBuff2), UTF8STRC("</Data></Cell>"));
						writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
						break;
					}

					i++;
				}
				writer.WriteLineC(UTF8STRC("   </Row>"));
			}
			
			writer.WriteLineC(UTF8STRC("  </Table>"));
			writer.WriteLineC(UTF8STRC(" </Worksheet>"));
			db->CloseReader(r);
		}
		j++;
	}
	writer.WriteLineC(UTF8STRC("</Workbook>"));
	LIST_FREE_STRING(&names);

	MemFree(lineBuff2);
	MemFree(lineBuff1);
	return true;
}
