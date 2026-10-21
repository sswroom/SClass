#include "Stdafx.h"
#include "AI/AIReadDBSkill.h"
#include "DB/TableDef.h"
#include "DB/DBReader.h"
#include "Text/JSONBuilder.h"

Bool AI::AIReadDBSkill::GetUIntOSInput(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, UIntOS defaultValue, OutParam<UIntOS> value)
{
	NN<Data::VariItem> item;
	if (!input->GetItem(name).SetTo(item))
	{
		value.Set(defaultValue);
		return true;
	}

	UInt64 u64Val;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::I64:
	{
		Int64 i64Val = item->GetAsI64();
		if (i64Val < 0)
			return false;
		u64Val = (UInt64)i64Val;
		break;
	}
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::U64:
		u64Val = item->GetAsU64();
		break;
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}

	UIntOS osVal = (UIntOS)u64Val;
	if ((UInt64)osVal != u64Val)
		return false;
	value.Set(osVal);
	return true;
}

AI::AIReadDBSkill::AIReadDBSkill(NN<DB::ReadingDB> db, Text::CStringNN schemaName) : db(db), schemaName(Text::String::New(schemaName))
{
}

AI::AIReadDBSkill::~AIReadDBSkill()
{
	this->schemaName->Release();
}

Text::CStringNN AI::AIReadDBSkill::GetName() const
{
	return CSTR("read_database");
}

void AI::AIReadDBSkill::GetDescription(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Read tables from schema "));
	sb->Append(this->schemaName);
	sb->AppendC(UTF8STRC(". Use list_tables to list tables. Use read_table with tableName, optional SQL-like condition, offset and maxCount to inspect rows. Conditions support comparisons, IN, BETWEEN and AND; for example site_code = 'SK-03'. Filter by condition instead of reading many rows. maxCount is limited to 100 and defaults to 20. Results are returned as JSON in data."));
}

void AI::AIReadDBSkill::GetParameters(NN<Net::OpenAIToolParameters> parameters) const
{
	parameters->AddParameter(CSTR("action"), CSTR("list_tables or read_table"), Net::OpenAIToolParameterType::String, true);
	parameters->AddParameter(CSTR("tableName"), CSTR("Table name; required for read_table"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("condition"), CSTR("Optional SQL-like filter, e.g. site_code = 'SK-03'"), Net::OpenAIToolParameterType::String, false);
	parameters->AddParameter(CSTR("offset"), CSTR("Row offset; defaults to 0"), Net::OpenAIToolParameterType::Integer, false);
	parameters->AddParameter(CSTR("maxCount"), CSTR("Maximum rows to return (1-100); defaults to 20"), Net::OpenAIToolParameterType::Integer, false);
}

Text::CStringNN AI::AIReadDBSkill::GetSchemaName() const
{
	return this->schemaName->ToCString();
}

UIntOS AI::AIReadDBSkill::QueryTableNames(NN<Data::ArrayListStringNN> names) const
{
	return this->db->QueryTableNames(this->schemaName->ToCString(), names);
}

Optional<DB::TableDef> AI::AIReadDBSkill::GetTableDef(Text::CStringNN tableName) const
{
	return this->db->GetTableDef(this->schemaName->ToCString(), tableName);
}

Optional<DB::DBReader> AI::AIReadDBSkill::QueryTableData(Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition) const
{
	return this->db->QueryTableData(this->schemaName->ToCString(), tableName, columnNames, dataOfst, maxCnt, ordering, condition);
}

void AI::AIReadDBSkill::CloseReader(NN<DB::DBReader> reader) const
{
	this->db->CloseReader(reader);
}

Bool AI::AIReadDBSkill::Invoke(NN<Data::VariObject> input, NN<Data::VariObject> output)
{
	NN<Data::VariItem> actionItem;
	NN<Text::String> action;
	if (!input->GetItem(U8STR("action")).SetTo(actionItem) || !actionItem->GetAsNewString().SetTo(action))
		return false;

	Bool listTables = action->Equals(UTF8STRC("list_tables"));
	Bool readTable = action->Equals(UTF8STRC("read_table"));
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

	if (!readTable)
		return false;

	NN<Data::VariItem> tableNameItem;
	NN<Text::String> tableName;
	if (!input->GetItem(U8STR("tableName")).SetTo(tableNameItem) || !tableNameItem->GetAsNewString().SetTo(tableName))
		return false;

	UIntOS dataOfst;
	UIntOS maxCnt;
	if (!GetUIntOSInput(input, U8STR("offset"), 0, dataOfst) || !GetUIntOSInput(input, U8STR("maxCount"), 20, maxCnt) || maxCnt == 0 || maxCnt > 100)
	{
		tableName->Release();
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
			return false;
		}
		if (!this->GetTableDef(tableName->ToCString()).SetTo(tableDef))
		{
			conditionText->Release();
			tableName->Release();
			return false;
		}
		condition = Data::QueryConditions::ParseStr(conditionText->ToCString(), tableDef->GetSQLType(), this->db->GetTzQhr());
		conditionText->Release();
		tableDef.Delete();
		if (condition.IsNull())
		{
			tableName->Release();
			return false;
		}
	}

	NN<DB::DBReader> reader;
	if (!this->QueryTableData(tableName->ToCString(), nullptr, dataOfst, maxCnt, nullptr, condition).SetTo(reader))
	{
		condition.Delete();
		tableName->Release();
		return false;
	}
	condition.Delete();
	tableName->Release();

	Text::JSONBuilder json(Text::JSONBuilder::OT_ARRAY);
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
				return false;
			}
			NN<Data::VariItem> item = Data::VariItem::NewUnknown();
			if (!reader->GetVariItem(i, item) || !json.ObjectAddVarItem(CSTRP(nameBuff, nameEnd), item))
			{
				item.Delete();
				this->CloseReader(reader);
				return false;
			}
			item.Delete();
			i++;
		}
		json.ObjectEnd();
	}
	this->CloseReader(reader);
	NN<Text::String> dataStr = Text::String::New(json.Build());
	output->SetItemStr(U8STR("data"), dataStr);
	dataStr->Release();
	return true;
}
