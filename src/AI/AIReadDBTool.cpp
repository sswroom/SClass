#include "Stdafx.h"
#include "AI/AIReadDBTool.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Exporter/XLSXExporter.h"
#include "IO/Path.h"
#include "Text/JSONBuilder.h"
#include "Text/SpreadSheet/Workbook.h"

//#define VERBOSE

AI::AIReadDBTool::AIReadDBTool(NN<DB::ReadingDB> db, Text::CStringNN schemaName, Text::CString filePath, Bool allowXlsx, Bool allowChart)
{
	this->db = db;
	this->schemaName = Text::String::New(schemaName);
	this->filePath = Text::String::NewOrNull(filePath);
	this->allowXlsx = allowXlsx;
	this->allowChart = allowChart;
}

AI::AIReadDBTool::~AIReadDBTool()
{
	this->schemaName->Release();
	OPTSTR_DEL(this->filePath)
}

Text::CStringNN AI::AIReadDBTool::GetName() const
{
	return CSTR("read_database");
}

void AI::AIReadDBTool::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Read tables from schema "));
	sb->Append(this->schemaName);
	sb->AppendC(UTF8STRC(". Use list_tables to list tables. Use read_table with tableName, optional SQL-like condition, offset and maxCount to inspect rows."));
	sb->AppendC(UTF8STRC(" Conditions support comparisons, IN, BETWEEN and AND; for example site_code = 'SK-03'."));
	sb->AppendC(UTF8STRC(" Filter by condition instead of reading many rows. maxCount is limited to 100 and defaults to 20. Results are returned as JSON in data."));
}

void AI::AIReadDBTool::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
	if (this->db->IsFullConn())
	{
		DB::SQLType sqlType = NN<DB::DBConn>::ConvertFrom(this->db)->GetSQLType();
		Text::StringBuilderUTF8 sbSQL;
		Text::StringBuilderUTF8 sbParam;
		sbSQL.Append(CSTR("SQL query in "));
		sbSQL.Append(DB::SQLTypeGetName(sqlType));
		sbSQL.Append(CSTR(" syntax; required for execute_sql"));

		sbParam.AppendC(UTF8STRC("list_tables, execute_sql"));
		if (this->filePath.NotNull())
		{
			if (this->allowXlsx)
			{
				sbParam.AppendC(UTF8STRC(", sql2xlsx"));
				sbSQL.Append(CSTR(", sql2xlsx"));
			}
			if (this->allowChart)
			{
				sbParam.AppendC(UTF8STRC(", sql2chart"));
				sbSQL.Append(CSTR(", sql2chart"));
			}
		}
		sbParam.AppendC(UTF8STRC(", or read_table"));
		parameters->AddParameter(CSTR("action"), sbParam.ToCString(), Net::OpenAIToolParameterType::String, true);

		if (DB::DBUtil::HasSchema(sqlType))
		{
			sbSQL.Append(CSTR("; requires schema name"));
		}
		parameters->AddParameter(CSTR("sql"), sbSQL.ToCString(), Net::OpenAIToolParameterType::String, false);
		if (this->filePath.NotNull())
		{
			if (this->allowXlsx || this->allowChart)
			{
				sbParam.ClearStr();
				sbParam.Append(CSTR("File Name without extension; required for "));
				if (this->allowXlsx)
				{
					sbParam.Append(CSTR("sql2xlsx"));
				}
				if (this->allowChart)
				{
					if (this->allowXlsx)
					{
						sbParam.Append(CSTR(" and "));
					}
					sbParam.Append(CSTR("sql2chart"));
				}
				parameters->AddParameter(CSTR("fileName"), sbParam.ToCString(), Net::OpenAIToolParameterType::String, false);
			}
			if (this->allowXlsx)
			{
				parameters->AddParameter(CSTR("sheetName"), CSTR("Sheet Name; required for sql2xlsx"), Net::OpenAIToolParameterType::String, false);
			}
		}
	}
	else
	{
		parameters->AddParameter(CSTR("action"), CSTR("list_tables or read_table"), Net::OpenAIToolParameterType::String, true);
	}
	parameters->AddParameter(CSTR("tableName"), CSTR("Table name; required for read_table"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("condition"), CSTR("Optional SQL-like filter, e.g. site_code = 'SK-03'"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("ordering"), CSTR("Optional ordering, e.g. columnName ASC"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("offset"), CSTR("Row offset; defaults to 0"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("maxCount"), CSTR("Maximum rows to return (1-100); defaults to 20"), Net::OpenAIToolParameterType::Integer, false);
}

Text::CStringNN AI::AIReadDBTool::GetSchemaName() const
{
	return this->schemaName->ToCString();
}

UIntOS AI::AIReadDBTool::QueryTableNames(NN<Data::ArrayListStringNN> names) const
{
	return this->db->QueryTableNames(this->schemaName->ToCString(), names);
}

Optional<DB::TableDef> AI::AIReadDBTool::GetTableDef(Text::CStringNN tableName) const
{
	return this->db->GetTableDef(this->schemaName->ToCString(), tableName);
}

Optional<DB::DBReader> AI::AIReadDBTool::QueryTableData(Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) const
{
#ifdef VERBOSE
	printf("AIReadDBTool: QueryTableData tableName = %s, dataOfst = %llu, maxCnt = %llu\n", tableName.v.Ptr(), dataOfst, maxCnt);
#endif
	return this->db->QueryTableData(this->schemaName->ToCString(), tableName, columnNames, dataOfst, maxCnt, ordering, condition);
}

void AI::AIReadDBTool::CloseReader(NN<DB::DBReader> reader) const
{
	this->db->CloseReader(reader);
}

Bool AI::AIReadDBTool::Invoke(Text::CStringNN sessionId, NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> actionItem;
	NN<Text::String> action;
	if (!input->GetItem(U8STR("action")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(action))
	{
		printf("AIReadDBTool: action not specified or invalid\n");
		return false;
	}

#ifdef VERBOSE
	printf("AIReadDBTool: action = %s\n", action->v.Ptr());
#endif
	Bool sqlAction = action->Equals(UTF8STRC("execute_sql"));
	Bool listTables = action->Equals(UTF8STRC("list_tables"));
	Bool readTable = action->Equals(UTF8STRC("read_table"));
	Bool sql2xlsx = action->Equals(UTF8STRC("sql2xlsx"));
	Bool sql2chart = action->Equals(UTF8STRC("sql2chart"));
	action->Release();
	if (listTables)
	{
		Data::ArrayListStringNN names;
		this->QueryTableNames(names);
		Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
		UIntOS i = 0;
		UIntOS count = names.GetCount();
		while (i < count)
		{
			json.ArrayAddStr(names.GetItemNoCheck(i));
			i++;
		}
		names.FreeAll();
		NN<Text::String> dataStr = Text::String::New(json.Build());
		output->SetItemStr(U8STR("data"), dataStr);
		dataStr->Release();
		return true;
	}

	if (sqlAction || sql2xlsx || sql2chart)
	{
		if (!this->db->IsFullConn())
			return false;
		NN<DB::DBConn> conn = NN<DB::DBConn>::ConvertFrom(this->db);
		NN<Text::String> sql;
		NN<Data::VariItem> sqlItem;
		if (!input->GetItem(U8STR("sql")).SetTo(sqlItem) || !sqlItem->GetAsNewString().SetTo(sql))
		{
			return false;
		}
		if (!sql->StartsWithICase(CSTR("SELECT ")))
		{
			sql->Release();
			return false;
		}
#ifdef VERBOSE
		printf("AIReadDBTool: Execute SQL: %s\n", sql->v.Ptr());
#endif
		NN<DB::DBReader> r;
		if (!conn->ExecuteReader(sql->ToCString()).SetTo(r))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Failed to execute SQL: "));
			conn->GetLastErrorMsg(sb);
			NN<Text::String> dataStr = Text::String::New(sb.ToCString());
			output->SetItemStr(U8STR("data"), dataStr);
			dataStr->Release();
			sql->Release();
#ifdef VERBOSE
			printf("AIReadDBTool: %s\n", sb.v.Ptr());
#endif
			return true;
		}
		if (sql2xlsx)
		{
			NN<Text::String> fileName;
			NN<Text::String> sheetName;
			if (!input->GetItem(U8STR("fileName")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(fileName))
			{
				sql->Release();
				return false;
			}
			if (!input->GetItem(U8STR("sheetName")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(sheetName))
			{
				sql->Release();
				fileName->Release();
				return false;
			}
			Text::SpreadSheet::Workbook workbook;
			NN<Text::SpreadSheet::Worksheet> sheet = workbook.AddWorksheet(sheetName);
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			NN<Text::String> s;
			UIntOS colCount = r->ColCount();
			UIntOS row = 0;
			UIntOS col = 0;
			UnsafeArray<DB::DBUtil::ColType> colTypes = MemAllocArr(DB::DBUtil::ColType, colCount);
			while (col < colCount)
			{
				if (r->GetName(col, sbuff).SetTo(sptr))
				{
					sheet->SetCellString(row, col, CSTRP(sbuff, sptr));
				}
				colTypes[col] = r->GetColType(col, nullptr);
				col++;
			}
			row++;
			while (r->ReadNext())
			{
				col = 0;
				while (col < colCount)
				{
					if (!r->IsNull(col))
					{
						switch (colTypes[col])
						{
						case DB::DBUtil::CT_Byte:
						case DB::DBUtil::CT_Int16:
						case DB::DBUtil::CT_UInt16:
						case DB::DBUtil::CT_Int32:
							sheet->SetCellInt32(row, col, r->GetInt32(col));
							break;
						case DB::DBUtil::CT_UInt32:
						case DB::DBUtil::CT_UInt64:
						case DB::DBUtil::CT_Int64:
							sheet->SetCellInt64(row, col, r->GetInt64(col));
							break;
						case DB::DBUtil::CT_Float:
						case DB::DBUtil::CT_Double:
						case DB::DBUtil::CT_Decimal:
							sheet->SetCellDouble(row, col, r->GetDblOrNAN(col));
							break;
						case DB::DBUtil::CT_Bool:
							sheet->SetCellString(row, col, r->GetBool(col) ? CSTR("Y") : CSTR("N"));
							break;
						case DB::DBUtil::CT_Date:
						case DB::DBUtil::CT_DateTimeTZ:
						case DB::DBUtil::CT_DateTime:
							sheet->SetCellTS(row, col, r->GetTimestamp(col).ToLocalTime());
							break;
						case DB::DBUtil::CT_Unknown:
						case DB::DBUtil::CT_UTF8Char:
						case DB::DBUtil::CT_UTF16Char:
						case DB::DBUtil::CT_UTF32Char:
						case DB::DBUtil::CT_VarUTF8Char:
						case DB::DBUtil::CT_VarUTF16Char:
						case DB::DBUtil::CT_VarUTF32Char:
						case DB::DBUtil::CT_Binary:
						case DB::DBUtil::CT_Vector:
						case DB::DBUtil::CT_UUID:
						default:
							if (r->GetNewStr(col).SetTo(s))
							{
								sheet->SetCellString(row, col, s);
								s->Release();
							}
							break;
						}
					}
					col++;
				}
				row++;
			}
			MemFreeArr(colTypes);
			Text::StringBuilderUTF8 sb;
			sb.AppendOpt(this->filePath);
			if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
			{
				sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			}
			Data::Timestamp fileTime = Data::Timestamp::Now();
			sb.AppendTS(fileTime, "yyyyMMdd");
			IO::Path::CreateDirectory(sb.ToCString());
			sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
			Text::StringBuilderUTF8 exportFileName;
			exportFileName.Append(sessionId);
			exportFileName.AppendUTF8Char('_');
			exportFileName.Append(fileName);
			sb.Append(exportFileName.ToCString());
			sb.Append(CSTR(".xlsx"));
			Bool succ = false;
			{
				Exporter::XLSXExporter exporter;
				succ = exporter.ExportNewFile(sb.ToCString(), workbook, nullptr);
			}
			if (!succ)
			{
				sb.ClearStr();
				sb.Append(CSTR("Failed to export to "));
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx file"));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("data"), s);
				s->Release();
			}
			else
			{
				sb.ClearStr();
				sb.Append(CSTR("Attachment "));
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx added to user"));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("data"), s);
				s->Release();

				sb.ClearStr();
				sb.AppendTS(fileTime, "yyyyMMdd");
				sb.AppendUTF8Char('/');
				sb.Append(exportFileName.ToCString());
				sb.Append(CSTR(".xlsx"));
				s = Text::String::New(sb.ToCString());
				output->SetItemStr(U8STR("attachment"), s);
				s->Release();
			}

			conn->CloseReader(r);
			fileName->Release();
			sheetName->Release();
			sql->Release();
			return true;
		}
		else if (sql2chart)
		{
			///////////////////////////////
			sql->Release();
			conn->CloseReader(r);
			return false;
		}
		else
		{
			Data::VariItem item;
			Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
			UIntOS colCount = r->ColCount();
			while (r->ReadNext())
			{
				json.ArrayBeginObject();
				UIntOS i = 0;
				while (i < colCount)
				{
					UTF8Char nameBuff[512];
					UnsafeArray<UTF8Char> nameEnd;
					if (!r->GetName(i, nameBuff).SetTo(nameEnd))
					{
						conn->CloseReader(r);
						printf("AIReadDBTool: failed to get column name at index %d\n", (Int32)i);
						return false;
					}
					if (!r->GetVariItem(i, item) || !json.ObjectAddVarItem(CSTRP(nameBuff, nameEnd), item))
					{
						conn->CloseReader(r);
						printf("AIReadDBTool: failed to get column value at index %d\n", (Int32)i);
						return false;
					}
					i++;
				}
				json.ObjectEnd();
			}
			conn->CloseReader(r);
			NN<Text::String> dataStr;
			dataStr = Text::String::New(json.Build());
			output->SetItemStr(U8STR("data"), dataStr);
			dataStr->Release();
			sql->Release();
			return true;
		}
	}

	if (!readTable)
		return false;

	NN<Data::VariItem> tableNameItem;
	NN<Text::String> tableName;
	Text::CString ordering = nullptr;
	Text::StringBuilderUTF8 orderingSb;
	if (!input->GetItem(U8STR("tableName")).SetTo(tableNameItem) || !tableNameItem->GetAsNewString().SetTo(tableName))
	{
		printf("AIReadDBTool: tableName not specified or invalid\n");
		return false;
	}

	NN<Data::VariItem> orderingItem;
	if (input->GetItem(U8STR("ordering")).SetTo(orderingItem))
	{
		orderingItem->GetAsString(orderingSb);
		if (orderingSb.GetLength() > 0)
		{
			ordering = orderingSb.ToCString();
		}
	}

	UIntOS dataOfst;
	UIntOS maxCnt;
	if (!GetUIntOSInput(input, U8STR("offset"), 0, dataOfst) || !GetUIntOSInput(input, U8STR("maxCount"), 20, maxCnt) || maxCnt == 0 || maxCnt > 100)
	{
		tableName->Release();
		printf("AIReadDBTool: invalid offset or maxCount\n");
		return false;
	}

	Optional<Data::QueryConditions> condition = nullptr;
	NN<Data::VariItem> conditionItem;
	if (input->GetItem(U8STR("condition")).SetTo(conditionItem))
	{
		NN<Text::String> conditionText;
		NN<DB::TableDef> tableDef;
		if (!conditionItem->GetAsNewString().SetTo(conditionText))
		{
			tableName->Release();
			printf("AIReadDBTool: condition is not a valid string\n");
			return false;
		}
		if (!this->GetTableDef(tableName->ToCString()).SetTo(tableDef))
		{
			conditionText->Release();
			tableName->Release();
			printf("AIReadDBTool: table not found: %s\n", tableName->v.Ptr());
			return false;
		}
		condition = Data::QueryConditions::ParseStr(conditionText->ToCString(), tableDef->GetSQLType(), this->db->GetTzQhr());
		conditionText->Release();
		tableDef.Delete();
		if (condition.IsNull())
		{
			tableName->Release();
			printf("AIReadDBTool: failed to parse condition: %s\n", conditionText->v.Ptr());
			return false;
		}
	}

	NN<DB::DBReader> reader;
	if (!this->QueryTableData(tableName->ToCString(), nullptr, dataOfst, maxCnt, ordering, condition).SetTo(reader))
	{
		condition.Delete();
		tableName->Release();
		printf("AIReadDBTool: failed to query table data: %s\n", tableName->v.Ptr());
		return false;
	}
	condition.Delete();
	tableName->Release();

	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
	Data::VariItem item;
	UIntOS colCount = reader->ColCount();
	while (reader->ReadNext())
	{
		json.ArrayBeginObject();
		UIntOS i = 0;
		while (i < colCount)
		{
			UTF8Char nameBuff[512];
			UnsafeArray<UTF8Char> nameEnd;
			if (!reader->GetName(i, nameBuff).SetTo(nameEnd))
			{
				this->CloseReader(reader);
				printf("AIReadDBTool: failed to get column name at index %d\n", (Int32)i);
				return false;
			}
			if (!reader->GetVariItem(i, item) || !json.ObjectAddVarItem(CSTRP(nameBuff, nameEnd), item))
			{
				this->CloseReader(reader);
				printf("AIReadDBTool: failed to get column value at index %d\n", (Int32)i);
				return false;
			}
			i++;
		}
		json.ObjectEnd();
	}
	this->CloseReader(reader);
	NN<Text::String> dataStr;
	dataStr = Text::String::New(json.Build());
	output->SetItemStr(U8STR("data"), dataStr);
	dataStr->Release();
	return true;
}
