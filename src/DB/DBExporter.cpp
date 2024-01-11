#include "Stdafx.h"
#include "DB/DBExporter.h"
#include "DB/DBReader.h"
#include "DB/SQLBuilder.h"
#include "DB/SQLGenerator.h"
#include "Exporter/ExcelXMLExporter.h"
#include "Exporter/XLSXExporter.h"
#include "IO/BufferedOutputStream.h"
#include "IO/StreamWriter.h"
#include "Text/XML.h"
#include "Text/SpreadSheet/Workbook.h"

Data::Class *DB::DBExporter::CreateTableClass(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName)
{
	DB::TableDef *tab = db->GetTableDef(schema, tableName);
	if (tab)
	{
		Data::Class *cls = tab->CreateTableClass().Ptr();
		DEL_CLASS(tab);
		return cls;
	}

	NotNullPtr<DB::DBReader> r;
	if (db->QueryTableData(schema, tableName, 0, 0, 1, CSTR_NULL, 0).SetTo(r))
	{
		Data::Class *cls = r->CreateClass().Ptr();
		db->CloseReader(r);
		return cls;
	}
	return 0;
}

Bool DB::DBExporter::GenerateInsertSQLs(NotNullPtr<DB::ReadingDB> db, DB::SQLType sqlType, Bool axisAware, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm)
{
	DB::SQLBuilder sql(sqlType, axisAware, 0);
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	while (r->ReadNext())
	{
		sql.Clear();			
		DB::SQLGenerator::GenInsertCmd(sql, schema, tableName, r);
		sql.AppendCmdC(CSTR(";\r\n"));
		if (outStm->WriteCont(sql.ToString(), sql.GetLength()) != sql.GetLength())
		{
			db->CloseReader(r);
			return false;
		}
	}
	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::GenerateCSV(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, Text::CStringNN nullText, NotNullPtr<IO::Stream> outStm, UInt32 codePage)
{
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt colCnt;
	UOSInt i;
	Bool firstCol;

	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);
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
			sptr = nullText.ConcatTo(sptr);
		}
		i++;
	}
	if (!writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2)))
	{
		MemFree(lineBuff2);
		MemFree(lineBuff1);

		db->CloseReader(r);
		return false;
	}

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
				sptr = nullText.ConcatTo(sptr);
			}
			i++;
		}
		if (!writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2)))
		{
			MemFree(lineBuff2);
			MemFree(lineBuff1);

			db->CloseReader(r);
			return false;
		}
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::GenerateSQLite(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<DB::SQLiteFile> file, Text::StringBuilderUTF8 *sbError)
{
	NotNullPtr<Text::StringBuilderUTF8> sb;
	NotNullPtr<DB::TableDef> table;
	if (!table.Set(db->GetTableDef(schema, tableName)))
	{
		if (sbError)
			sbError->Append(CSTR("Error in getting table definition"));
		return false;
	}
	DB::SQLBuilder sql(file->GetSQLType(), false, file->GetTzQhr());
	DB::SQLGenerator::GenCreateTableCmd(sql, CSTR_NULL, tableName, table, false);
	if (file->ExecuteNonQuery(sql.ToCString()) <= -2)
	{
		if (sb.Set(sbError))
		{
			sb->AppendC(UTF8STRC("Error in creating table: "));
			file->GetLastErrorMsg(sb);
			sb->AppendC(UTF8STRC("\r\nSQL Command: "));
			sb->Append(sql.ToCString());
		}
		table.Delete();
		return false;
	}
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		if (sb.Set(sbError))
		{
			sb->Append(CSTR("Error in reading table data\r\n"));
			db->GetLastErrorMsg(sb);
		}
		table.Delete();
		return false;
	}
	Bool succ = true;
	void *tran = file->BeginTransaction();
	while (r->ReadNext())
	{
		sql.Clear();
		DB::SQLGenerator::GenInsertCmd(sql, CSTR_NULL, tableName, table.Ptr(), r);
		if (file->ExecuteNonQuery(sql.ToCString()) != 1)
		{
			if (sb.Set(sbError))
			{
				sb->AppendC(UTF8STRC("Error in executing cmd: "));
				file->GetLastErrorMsg(sb);
				sb->AppendC(UTF8STRC("\r\nSQL Command: "));
				sb->Append(sql.ToCString());
			}
			succ = false;
			break;
		}
	}
	db->CloseReader(r);
	file->Commit(tran);
	table.Delete();
	return succ;
}

Bool DB::DBExporter::GenerateHTML(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm, UInt32 codePage)
{
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt colCnt;
	UOSInt i;
	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	writer.WriteLineC(UTF8STRC("<html>"));
	writer.WriteLineC(UTF8STRC("<head>"));
	writer.WriteStrC(UTF8STRC("<title>"));
	sptr2 = db->GetSourceName(lineBuff1);
	sptr = lineBuff1;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr2 = Text::XML::ToXMLText(lineBuff2, sptr);
	writer.WriteLineC(lineBuff2, (UOSInt)(sptr2 - lineBuff2));
	writer.WriteLineC(UTF8STRC("</title>"));
	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=")), codePage), UTF8STRC("\">"));
	writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer.WriteLineC(UTF8STRC("</head>"));
	sptr2 = db->GetSourceName(lineBuff2);
	sptr = lineBuff2;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr = Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("<body><h1>")), sptr);
	writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer.WriteLineC(UTF8STRC("</h1>"));
	writer.WriteLineC(UTF8STRC("<table border=1 cellspacing=1 cellpadding=0><tr>"));

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
	writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));

	while (r->ReadNext())
	{
		writer.WriteLineC(UTF8STRC("<tr>"));
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
		writer.WriteLineC(lineBuff2, (UOSInt)(sptr - lineBuff2));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer.WriteLineC(UTF8STRC("</table>"));
	writer.WriteLineC(UTF8STRC("</body></html>"));

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::GeneratePList(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::Stream> outStm, UInt32 codePage)
{
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt colCnt;
	UOSInt i;
	UOSInt colSize;
	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), codePage), UTF8STRC("\"?>"));
	writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer.WriteLineC(UTF8STRC("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"));
	writer.WriteLineC(UTF8STRC("<plist version=\"1.0\">"));
	writer.WriteLineC(UTF8STRC("<array>"));

	sptr = lineBuff2;
	colCnt = r->ColCount();
	while (r->ReadNext())
	{
		writer.WriteLineC(UTF8STRC("    <dict>"));
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			r->GetName(i, lineBuff1);
			sptr2 = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff2, UTF8STRC("        <key>")), lineBuff1), UTF8STRC("</key>"));
			writer.WriteLineC(lineBuff2, (UOSInt)(sptr2 - lineBuff2));

			DB::DBUtil::ColType ct = r->GetColType(i, colSize);
			switch (ct)
			{
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
				sptr = Text::StrConcatC(r->GetTimestamp(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd HH:mm:ss"), UTF8STRC("</string>"));
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Date:
				sptr = Text::StrConcatC(r->GetDate(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd"), UTF8STRC("</string>"));
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
			case DB::DBUtil::CT_Decimal:
				sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), r->GetDbl(i)), UTF8STRC("</string>"));
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_UInt16:
				sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt32(i)), UTF8STRC("</integer>"));
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Int64:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt64:
				sptr = Text::StrConcatC(Text::StrInt64(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt64(i)), UTF8STRC("</integer>"));
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			case DB::DBUtil::CT_Bool:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Binary:
			case DB::DBUtil::CT_Vector:

			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
			case DB::DBUtil::CT_Unknown:
			case DB::DBUtil::CT_UUID:
			default:
				if (r->GetStr(i, lineBuff2, 65536))
				{
					sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), lineBuff2), UTF8STRC("</string>"));
				}
				else
				{
					sptr = Text::StrConcatC(lineBuff1, UTF8STRC("        <string>(null)</string>"));
				}
				writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
				break;
			}

			i++;
		}
		writer.WriteLineC(UTF8STRC("    </dict>"));
	}
	
	MemFree(lineBuff2);
	MemFree(lineBuff1);

	writer.WriteLineC(UTF8STRC("</array>"));
	writer.WriteLineC(UTF8STRC("</plist>"));

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::AppendWorksheet(NotNullPtr<Text::SpreadSheet::Workbook> wb, NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, Text::StringBuilderUTF8 *sbError)
{
	UTF8Char sbuff[4096];
	UTF8Char *sptr;
	NotNullPtr<Text::StringBuilderUTF8> sb;
	DB::TableDef *table = db->GetTableDef(schema, tableName);
	NotNullPtr<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		if (sb.Set(sbError))
		{
			sb->Append(CSTR("Error in reading table data\r\n"));
			db->GetLastErrorMsg(sb);
		}
		SDEL_CLASS(table);
		return false;
	}
	NotNullPtr<Text::SpreadSheet::Worksheet> ws = wb->AddWorksheet(tableName);
	Text::SpreadSheet::CellStyle style(0);
	NotNullPtr<Text::SpreadSheet::CellStyle> timeStyle;
	UOSInt i = 0;
	UOSInt j = r->ColCount();
	UOSInt row;
	while (i < j)
	{
		sptr = r->GetName(i, sbuff);
		if (sptr)
			ws->SetCellString(0, i, CSTRP(sbuff, sptr));
		i++;
	}
	row = 1;
	while (r->ReadNext())
	{
		i = 0;
		while (i < j)
		{
			if (!r->IsNull(i))
			{
				UOSInt colSize;
				DB::DBUtil::ColType ct = r->GetColType(i, colSize);
				switch (ct)
				{
				case DB::DBUtil::CT_Date:
				case DB::DBUtil::CT_DateTime:
				case DB::DBUtil::CT_DateTimeTZ:
					if (colSize < 3)
					{
						style.SetDataFormat(CSTR("YYYY-MM-DD HH:MM:SS"));
					}
					else
					{
						style.SetDataFormat(CSTR("YYYY-MM-DD HH:MM:SS.000"));
					}
					timeStyle = wb->FindOrCreateStyle(style);
					ws->SetCellTS(row, i, timeStyle.Ptr(), r->GetTimestamp(i));
					break;
				case DB::DBUtil::CT_Double:
				case DB::DBUtil::CT_Float:
				case DB::DBUtil::CT_Decimal:
					ws->SetCellDouble(row, i, r->GetDbl(i));
					break;
				case DB::DBUtil::CT_Byte:
				case DB::DBUtil::CT_Int16:
				case DB::DBUtil::CT_Int32:
				case DB::DBUtil::CT_UInt16:
					ws->SetCellInt32(row, i, r->GetInt32(i));
					break;
				case DB::DBUtil::CT_Int64:
				case DB::DBUtil::CT_UInt32:
					ws->SetCellInt64(row, i, r->GetInt64(i));
					break;
				case DB::DBUtil::CT_UInt64:
					ws->SetCellDouble(row, i, (Double)(UInt64)r->GetInt64(i));
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
					sptr = r->GetStr(i, sbuff, sizeof(sbuff));
					if (sptr)
					{
						ws->SetCellString(row, i, CSTRP(sbuff, sptr));
					}
					break;
				}
			}
			i++;
		}
		row++;
	}
	db->CloseReader(r);
	SDEL_CLASS(table);
	return true;
}

Bool DB::DBExporter::GenerateXLSX(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::SeekableStream> outStm, Text::StringBuilderUTF8 *sbError)
{
	Text::SpreadSheet::Workbook wb;
	wb.AddDefaultStyles();
	if (!AppendWorksheet(wb, db, schema, tableName, cond, sbError))
		return false;

	Exporter::XLSXExporter exporter;
	return exporter.ExportFile(outStm, outStm->GetSourceNameObj()->ToCString(), wb, 0);
}

Bool DB::DBExporter::GenerateExcelXML(NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Data::QueryConditions *cond, NotNullPtr<IO::SeekableStream> outStm, Text::StringBuilderUTF8 *sbError)
{
	Text::SpreadSheet::Workbook wb;
	wb.AddDefaultStyles();
	if (!AppendWorksheet(wb, db, schema, tableName, cond, sbError))
		return false;

	Exporter::ExcelXMLExporter exporter;
	return exporter.ExportFile(outStm, outStm->GetSourceNameObj()->ToCString(), wb, 0);
}

Bool DB::DBExporter::GenerateExcelXMLAllTables(NotNullPtr<DB::ReadingDB> db, Text::CString schema, NotNullPtr<IO::Stream> outStm, UInt32 codePage)
{
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UOSInt colCnt;
	UOSInt i;

	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	NotNullPtr<DB::DBReader> r;

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), codePage), UTF8STRC("\"?>"));
	writer.WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer.WriteLineC(UTF8STRC("<?mso-application progid=\"Excel.Sheet\"?>"));
	writer.WriteLineC(UTF8STRC("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLineC(UTF8STRC(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\""));
	writer.WriteLineC(UTF8STRC(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLineC(UTF8STRC(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">"));

	Data::ArrayListStringNN names;
	db->QueryTableNames(CSTR_NULL, names);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = names.Iterator();
	while (it.HasNext())
	{
		NotNullPtr<Text::String> tableName = it.Next();
		if (db->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
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
	}
	writer.WriteLineC(UTF8STRC("</Workbook>"));
	names.FreeAll();

	MemFree(lineBuff2);
	MemFree(lineBuff1);
	return true;
}
