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

Optional<Data::Class> DB::DBExporter::CreateTableClass(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName)
{
	NN<DB::TableDef> tab;
	if (db->GetTableDef(schema, tableName).SetTo(tab))
	{
		NN<Data::Class> cls = tab->CreateTableClass();
		tab.Delete();
		return cls;
	}

	NN<DB::DBReader> r;
	if (db->QueryTableData(schema, tableName, 0, 0, 1, CSTR_NULL, 0).SetTo(r))
	{
		NN<Data::Class> cls = r->CreateClass();
		db->CloseReader(r);
		return cls;
	}
	return 0;
}

Bool DB::DBExporter::GenerateInsertSQLs(NN<DB::ReadingDB> db, DB::SQLType sqlType, Bool axisAware, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm)
{
	DB::SQLBuilder sql(sqlType, axisAware, 0);
	NN<DB::DBReader> r;
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

Bool DB::DBExporter::GenerateCSV(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, Text::CStringNN nullText, NN<IO::Stream> outStm, UInt32 codePage)
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UnsafeArray<UTF8Char> lineBuff1;
	UnsafeArray<UTF8Char> lineBuff2;
	UnsafeArray<UTF8Char> sptr;
	UOSInt colCnt;
	UOSInt i;
	Bool firstCol;

	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);
	writer.WriteSignature();

	lineBuff1 = MemAllocArr(UTF8Char, 65536);
	lineBuff2 = MemAllocArr(UTF8Char, 65536);

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

		if (r->GetName(i, lineBuff1).NotNull())
		{
			sptr = Text::StrToCSVRec(sptr, lineBuff1);
		}
		else
		{
			sptr = nullText.ConcatTo(sptr);
		}
		i++;
	}
	if (!writer.WriteLine(CSTRP(lineBuff2, sptr)))
	{
		MemFreeArr(lineBuff2);
		MemFreeArr(lineBuff1);

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

			if (r->GetStr(i, lineBuff1, 65536).NotNull())
			{
				sptr = Text::StrToCSVRec(sptr, lineBuff1);
			}
			else
			{
				sptr = nullText.ConcatTo(sptr);
			}
			i++;
		}
		if (!writer.WriteLine(CSTRP(lineBuff2, sptr)))
		{
			MemFreeArr(lineBuff2);
			MemFreeArr(lineBuff1);

			db->CloseReader(r);
			return false;
		}
	}
	
	MemFreeArr(lineBuff2);
	MemFreeArr(lineBuff1);

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::GenerateSQLite(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<DB::SQLiteFile> file, Optional<Text::StringBuilderUTF8> sbError)
{
	NN<Text::StringBuilderUTF8> sb;
	NN<DB::TableDef> table;
	if (!db->GetTableDef(schema, tableName).SetTo(table))
	{
		if (sbError.SetTo(sb))
			sb->Append(CSTR("Error in getting table definition"));
		return false;
	}
	DB::SQLBuilder sql(file->GetSQLType(), false, file->GetTzQhr());
	DB::SQLGenerator::GenCreateTableCmd(sql, CSTR_NULL, tableName, table, false);
	if (file->ExecuteNonQuery(sql.ToCString()) <= -2)
	{
		if (sbError.SetTo(sb))
		{
			sb->AppendC(UTF8STRC("Error in creating table: "));
			file->GetLastErrorMsg(sb);
			sb->AppendC(UTF8STRC("\r\nSQL Command: "));
			sb->Append(sql.ToCString());
		}
		table.Delete();
		return false;
	}
	NN<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		if (sbError.SetTo(sb))
		{
			sb->Append(CSTR("Error in reading table data\r\n"));
			db->GetLastErrorMsg(sb);
		}
		table.Delete();
		return false;
	}
	Bool succ = true;
	Optional<DB::DBTransaction> tran = file->BeginTransaction();
	NN<DB::DBTransaction> thisTran;
	while (r->ReadNext())
	{
		sql.Clear();
		DB::SQLGenerator::GenInsertCmd(sql, CSTR_NULL, tableName, table.Ptr(), r);
		if (file->ExecuteNonQuery(sql.ToCString()) != 1)
		{
			if (sbError.SetTo(sb))
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
	if (tran.SetTo(thisTran))
	{
		file->Commit(thisTran);
	}
	table.Delete();
	return succ;
}

Bool DB::DBExporter::GenerateHTML(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm, UInt32 codePage)
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UnsafeArray<UTF8Char> lineBuff1;
	UnsafeArray<UTF8Char> lineBuff2;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt colCnt;
	UOSInt i;
	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAllocArr(UTF8Char, 65536);
	lineBuff2 = MemAllocArr(UTF8Char, 65536);

	writer.WriteLine(CSTR("<html>"));
	writer.WriteLine(CSTR("<head>"));
	writer.Write(CSTR("<title>"));
	sptr2 = db->GetSourceName(lineBuff1);
	sptr = lineBuff1;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr2 = Text::XML::ToXMLText(lineBuff2, sptr);
	writer.WriteLine(CSTRP(lineBuff2, sptr2));
	writer.WriteLine(CSTR("</title>"));
	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=")), codePage), UTF8STRC("\">"));
	writer.WriteLine(CSTRP(lineBuff1, sptr));
	writer.WriteLine(CSTR("</head>"));
	sptr2 = db->GetSourceName(lineBuff2);
	sptr = lineBuff2;
	if ((i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptr2 - sptr), '\\')) != INVALID_INDEX)
		sptr = &sptr[i + 1];
	sptr = Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("<body><h1>")), sptr);
	writer.WriteLine(CSTRP(lineBuff1, sptr));
	writer.WriteLine(CSTR("</h1>"));
	writer.WriteLine(CSTR("<table border=1 cellspacing=1 cellpadding=0><tr>"));

	sptr = lineBuff2;
	colCnt = r->ColCount();
	i = 0;
	while (i < colCnt)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("<th>"));
		if (r->GetName(i, lineBuff1).NotNull())
		{
			sptr = Text::XML::ToXMLText(sptr, lineBuff1);
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("</th>"));
		i++;
	}
	sptr = Text::StrConcatC(sptr, UTF8STRC("</tr>"));
	writer.WriteLine(CSTRP(lineBuff2, sptr));

	while (r->ReadNext())
	{
		writer.WriteLine(CSTR("<tr>"));
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("<td>"));
			if (r->GetStr(i, lineBuff1, 65536).NotNull())
			{
				sptr = Text::XML::ToXMLText(sptr, lineBuff1);
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("</td>"));
			i++;
		}
		writer.WriteLine(CSTRP(lineBuff2, sptr));
	}
	
	MemFreeArr(lineBuff2);
	MemFreeArr(lineBuff1);

	writer.WriteLine(CSTR("</table>"));
	writer.WriteLine(CSTR("</body></html>"));

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::GeneratePList(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::Stream> outStm, UInt32 codePage)
{
	NN<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		return false;
	}
	UnsafeArray<UTF8Char> lineBuff1;
	UnsafeArray<UTF8Char> lineBuff2;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt colCnt;
	UOSInt i;
	UOSInt colSize;
	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAllocArr(UTF8Char, 65536);
	lineBuff2 = MemAllocArr(UTF8Char, 65536);

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), codePage), UTF8STRC("\"?>"));
	writer.WriteLine(CSTRP(lineBuff1, sptr));
	writer.WriteLine(CSTR("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">"));
	writer.WriteLine(CSTR("<plist version=\"1.0\">"));
	writer.WriteLine(CSTR("<array>"));

	sptr = lineBuff2;
	colCnt = r->ColCount();
	while (r->ReadNext())
	{
		writer.WriteLine(CSTR("    <dict>"));
		sptr = lineBuff2;
		colCnt = r->ColCount();
		i = 0;
		while (i < colCnt)
		{
			r->GetName(i, lineBuff1);
			sptr2 = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff2, UTF8STRC("        <key>")), lineBuff1), UTF8STRC("</key>"));
			writer.WriteLine(CSTRP(lineBuff2, sptr2));

			DB::DBUtil::ColType ct = r->GetColType(i, colSize);
			switch (ct)
			{
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
				sptr = Text::StrConcatC(r->GetTimestamp(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd HH:mm:ss"), UTF8STRC("</string>"));
				writer.WriteLine(CSTRP(lineBuff1, sptr));
				break;
			case DB::DBUtil::CT_Date:
				sptr = Text::StrConcatC(r->GetDate(i).ToString(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), "yyyy-MM-dd"), UTF8STRC("</string>"));
				writer.WriteLine(CSTRP(lineBuff1, sptr));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
			case DB::DBUtil::CT_Decimal:
				sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), r->GetDbl(i)), UTF8STRC("</string>"));
				writer.WriteLine(CSTRP(lineBuff1, sptr));
				break;
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_UInt16:
				sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt32(i)), UTF8STRC("</integer>"));
				writer.WriteLine(CSTRP(lineBuff1, sptr));
				break;
			case DB::DBUtil::CT_Int64:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt64:
				sptr = Text::StrConcatC(Text::StrInt64(Text::StrConcatC(lineBuff1, UTF8STRC("        <integer>")), r->GetInt64(i)), UTF8STRC("</integer>"));
				writer.WriteLine(CSTRP(lineBuff1, sptr));
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
				if (r->GetStr(i, lineBuff2, 65536).NotNull())
				{
					sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff1, UTF8STRC("        <string>")), lineBuff2), UTF8STRC("</string>"));
				}
				else
				{
					sptr = Text::StrConcatC(lineBuff1, UTF8STRC("        <string>(null)</string>"));
				}
				writer.WriteLine(CSTRP(lineBuff1, sptr));
				break;
			}

			i++;
		}
		writer.WriteLine(CSTR("    </dict>"));
	}
	
	MemFreeArr(lineBuff2);
	MemFreeArr(lineBuff1);

	writer.WriteLine(CSTR("</array>"));
	writer.WriteLine(CSTR("</plist>"));

	db->CloseReader(r);
	return true;
}

Bool DB::DBExporter::AppendWorksheet(NN<Text::SpreadSheet::Workbook> wb, NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, Optional<Text::StringBuilderUTF8> sbError)
{
	UTF8Char sbuff[4096];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::StringBuilderUTF8> sb;
	Optional<DB::TableDef> table = db->GetTableDef(schema, tableName);
	NN<DB::DBReader> r;
	if (!db->QueryTableData(schema, tableName, 0, 0, 0, CSTR_NULL, cond).SetTo(r))
	{
		if (sbError.SetTo(sb))
		{
			sb->Append(CSTR("Error in reading table data\r\n"));
			db->GetLastErrorMsg(sb);
		}
		table.Delete();
		return false;
	}
	NN<Text::SpreadSheet::Worksheet> ws = wb->AddWorksheet(tableName);
	Text::SpreadSheet::CellStyle style(0);
	NN<Text::SpreadSheet::CellStyle> timeStyle;
	UOSInt i = 0;
	UOSInt j = r->ColCount();
	UOSInt row;
	while (i < j)
	{
		if (r->GetName(i, sbuff).SetTo(sptr))
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
					if (r->GetStr(i, sbuff, sizeof(sbuff)).SetTo(sptr))
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
	table.Delete();
	return true;
}

Bool DB::DBExporter::GenerateXLSX(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::SeekableStream> outStm, Optional<Text::StringBuilderUTF8> sbError)
{
	Text::SpreadSheet::Workbook wb;
	wb.AddDefaultStyles();
	if (!AppendWorksheet(wb, db, schema, tableName, cond, sbError))
		return false;

	Exporter::XLSXExporter exporter;
	return exporter.ExportFile(outStm, outStm->GetSourceNameObj()->ToCString(), wb, 0);
}

Bool DB::DBExporter::GenerateExcelXML(NN<DB::ReadingDB> db, Text::CString schema, Text::CStringNN tableName, Optional<Data::QueryConditions> cond, NN<IO::SeekableStream> outStm, Optional<Text::StringBuilderUTF8> sbError)
{
	Text::SpreadSheet::Workbook wb;
	wb.AddDefaultStyles();
	if (!AppendWorksheet(wb, db, schema, tableName, cond, sbError))
		return false;

	Exporter::ExcelXMLExporter exporter;
	return exporter.ExportFile(outStm, outStm->GetSourceNameObj()->ToCString(), wb, 0);
}

Bool DB::DBExporter::GenerateExcelXMLAllTables(NN<DB::ReadingDB> db, Text::CString schema, NN<IO::Stream> outStm, UInt32 codePage)
{
	UnsafeArray<UTF8Char> lineBuff1;
	UnsafeArray<UTF8Char> lineBuff2;
	UnsafeArray<UTF8Char> sptr;
	UOSInt colCnt;
	UOSInt i;

	IO::BufferedOutputStream cstm(outStm, 65536);
	IO::StreamWriter writer(cstm, codePage);

	lineBuff1 = MemAllocArr(UTF8Char, 65536);
	lineBuff2 = MemAllocArr(UTF8Char, 65536);

	NN<DB::DBReader> r;

	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<?xml version=\"1.0\" encoding=\"")), codePage), UTF8STRC("\"?>"));
	writer.WriteLine(CSTRP(lineBuff1, sptr));
	writer.WriteLine(CSTR("<?mso-application progid=\"Excel.Sheet\"?>"));
	writer.WriteLine(CSTR("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLine(CSTR(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\""));
	writer.WriteLine(CSTR(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\""));
	writer.WriteLine(CSTR(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">"));

	Data::ArrayListStringNN names;
	db->QueryTableNames(CSTR_NULL, names);
	Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
	while (it.HasNext())
	{
		NN<Text::String> tableName = it.Next();
		if (db->QueryTableData(CSTR_NULL, tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			UOSInt ind = tableName->LastIndexOf('\\');
			sptr = Text::StrConcatC(Text::XML::ToAttrText(Text::StrConcatC(lineBuff1, UTF8STRC(" <Worksheet ss:Name=")), &tableName->v[ind + 1]), UTF8STRC(">"));
			Text::StrReplace(lineBuff1, '?', '_');
			Text::StrReplace(lineBuff1, '\\', '_');
			writer.WriteLine(CSTRP(lineBuff1, sptr));
			writer.WriteLine(CSTR("  <Table>"));

			writer.WriteLine(CSTR("   <Row>"));
			colCnt = r->ColCount();
			i = 0;
			while (i < colCnt)
			{
				if (r->GetName(i, lineBuff1).NotNull())
				{
					sptr = Text::StrConcatC(Text::XML::ToXMLText(Text::StrConcatC(lineBuff2, UTF8STRC("    <Cell><Data ss:Type=\"String\">")), lineBuff1), UTF8STRC("</Data></Cell>"));
					writer.WriteLine(CSTRP(lineBuff2, sptr));
				}
				else
				{
					writer.WriteLine(CSTR("    <Cell><Data ss:Type=\"String\"></Data></Cell>"));
				}
				i++;
			}
			writer.WriteLine(CSTR("   </Row>"));

			while (r->ReadNext())
			{
				writer.WriteLine(CSTR("   <Row>"));
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
						writer.WriteLine(CSTRP(lineBuff1, sptr));
						break;
					case DB::DBUtil::CT_Double:
					case DB::DBUtil::CT_Float:
					case DB::DBUtil::CT_Decimal:
						sptr = Text::StrConcatC(Text::StrDouble(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetDbl(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLine(CSTRP(lineBuff1, sptr));
						break;
					case DB::DBUtil::CT_Byte:
					case DB::DBUtil::CT_Int16:
					case DB::DBUtil::CT_Int32:
					case DB::DBUtil::CT_UInt16:
						sptr = Text::StrConcatC(Text::StrInt32(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetInt32(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLine(CSTRP(lineBuff1, sptr));
						break;
					case DB::DBUtil::CT_Int64:
					case DB::DBUtil::CT_UInt32:
						sptr = Text::StrConcatC(Text::StrInt64(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), r->GetInt64(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLine(CSTRP(lineBuff1, sptr));
						break;
					case DB::DBUtil::CT_UInt64:
						sptr = Text::StrConcatC(Text::StrUInt64(Text::StrConcatC(lineBuff1, UTF8STRC("    <Cell><Data ss:Type=\"Number\">")), (UInt64)r->GetInt64(i)), UTF8STRC("</Data></Cell>"));
						writer.WriteLine(CSTRP(lineBuff1, sptr));
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
						writer.WriteLine(CSTRP(lineBuff1, sptr));
						break;
					}

					i++;
				}
				writer.WriteLine(CSTR("   </Row>"));
			}
			
			writer.WriteLine(CSTR("  </Table>"));
			writer.WriteLine(CSTR(" </Worksheet>"));
			db->CloseReader(r);
		}
	}
	writer.WriteLine(CSTR("</Workbook>"));
	names.FreeAll();

	MemFreeArr(lineBuff2);
	MemFreeArr(lineBuff1);
	return true;
}
