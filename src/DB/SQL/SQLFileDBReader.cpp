#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/SQL/SQLFileDBReader.h"
#include "DB/SQL/SQLValueBool.h"
#include "DB/SQL/SQLValueF64.h"
#include "DB/SQL/SQLValueI32.h"
#include "DB/SQL/SQLValueI64.h"
#include "DB/SQL/SQLValueString.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

DB::SQL::SQLFileDBReader::SQLFileDBReader(NN<SQLFileDB::TableInfo> tableInfo, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Optional<Data::QueryConditions> condition, Int8 tzQhr) : colIndexMap(16)
{
	this->tableInfo = tableInfo;
	this->currRow = nullptr;
	this->firstInsertCmd = this->GetFirstInsertCmd();
	this->condition = condition;
	this->nextCmdIndex = 0;
	this->nextRowIndex = 0;
	this->nextDataOfst = dataOfst;
	this->rowReadCnt = 0;
	this->maxCnt = maxCnt;
	this->tzQhr = tzQhr;

	NN<SQLInsertCommand> insertCmd;
	if (this->firstInsertCmd.SetTo(insertCmd))
	{
		NN<Data::ArrayListStringNN> nnColNames;
		UIntOS i;
		UIntOS j;
		if (colNames.SetTo(nnColNames))
		{
			i = 0;
			j = nnColNames->GetCount();
			NN<Text::String> colName;
			Int32 srcCol;
			while (i < j)
			{
				if (nnColNames->GetItem(i).SetTo(colName))
				{
					srcCol = this->GetSourceColIndex(insertCmd, colName->ToCString());
					if (srcCol >= 0)
					{
						this->colIndexMap.Add((UInt32)srcCol);
					}
				}
				i++;
			}
		}
		else
		{
			i = 0;
			j = insertCmd->GetColumnCount();
			while (i < j)
			{
				this->colIndexMap.Add((UInt32)i);
				i++;
			}
		}
	}
}

DB::SQL::SQLFileDBReader::~SQLFileDBReader()
{
}

Optional<DB::SQL::SQLInsertCommand> DB::SQL::SQLFileDBReader::GetFirstInsertCmd() const
{
	UIntOS i = 0;
	UIntOS j = this->tableInfo->insertCmds.GetCount();
	NN<SQLInsertCommand> insertCmd;
	while (i < j)
	{
		if (this->tableInfo->insertCmds.GetItem(i).SetTo(insertCmd))
		{
			return insertCmd;
		}
		i++;
	}
	return nullptr;
}

Int32 DB::SQL::SQLFileDBReader::GetSourceColIndex(NN<SQLInsertCommand> insertCmd, Text::CStringNN colName) const
{
	UIntOS i = 0;
	UIntOS j = insertCmd->GetColumnCount();
	Optional<Text::String> optCol;
	NN<Text::String> srcCol;
	while (i < j)
	{
		optCol = insertCmd->GetColumn(i);
		if (optCol.SetTo(srcCol) && srcCol->Equals(colName))
		{
			return (Int32)i;
		}
		i++;
	}
	return -1;
}

Optional<DB::SQL::SQLValue> DB::SQL::SQLFileDBReader::GetSQLValue(UIntOS colIndex) const
{
	NN<Data::ArrayListNN<SQLValue>> row;
	if (!this->currRow.SetTo(row) || colIndex >= this->colIndexMap.GetCount())
	{
		return nullptr;
	}
	UInt32 srcCol = this->colIndexMap.GetItem(colIndex);
	if ((UIntOS)srcCol >= row->GetCount())
	{
		return nullptr;
	}
	return row->GetItem((UIntOS)srcCol);
}

Bool DB::SQL::SQLFileDBReader::ReadNext()
{
	if (this->maxCnt != 0 && this->rowReadCnt >= this->maxCnt)
	{
		return false;
	}

	UIntOS i;
	UIntOS j;
	NN<SQLInsertCommand> insertCmd;
	UIntOS rowCnt;
	Optional<Data::ArrayListNN<SQLValue>> row;
	NN<Data::QueryConditions> nnCondition;
	Bool valid;
	NN<Data::VariObject> obj;
	while (this->nextCmdIndex < this->tableInfo->insertCmds.GetCount())
	{
		i = this->nextCmdIndex;
		if (!this->tableInfo->insertCmds.GetItem(i).SetTo(insertCmd))
		{
			this->nextCmdIndex = i + 1;
			this->nextRowIndex = 0;
			continue;
		}

		j = this->nextRowIndex;
		rowCnt = insertCmd->GetRowCount();
		while (j < rowCnt)
		{
			row = insertCmd->GetRow(j);
			j++;
			this->nextRowIndex = j;
			if (this->nextDataOfst > 0)
			{
				this->nextDataOfst--;
				continue;
			}
			if (row.IsNull())
			{
				continue;
			}
			this->currRow = row;

			if (!this->condition.SetTo(nnCondition))
			{
				this->rowReadCnt++;
				return true;
			}
			obj = this->CreateVariObject();
			if (!nnCondition->IsValid(obj, valid) || valid)
			{
				obj.Delete();
				this->rowReadCnt++;
				return true;
			}
			obj.Delete();
		}
		this->nextCmdIndex = i + 1;
		this->nextRowIndex = 0;
	}
	return false;
}

UIntOS DB::SQL::SQLFileDBReader::ColCount()
{
	return this->colIndexMap.GetCount();
}

IntOS DB::SQL::SQLFileDBReader::GetRowChanged()
{
	return -1;
}

Int32 DB::SQL::SQLFileDBReader::GetInt32(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return 0;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::I32:
		return NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::I64:
		return (Int32)NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::F64:
		return (Int32)NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::Bool:
		return NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue() ? 1 : 0;
	case SQLValue::ValueType::String:
		return Text::StrToInt32(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->v);
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return 0;
	}
}

Int64 DB::SQL::SQLFileDBReader::GetInt64(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return 0;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::I32:
		return NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::I64:
		return NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::F64:
		return (Int64)NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::Bool:
		return NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue() ? 1 : 0;
	case SQLValue::ValueType::String:
		return Text::StrToInt64(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->v);
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return 0;
	}
}

UnsafeArrayOpt<WChar> DB::SQL::SQLFileDBReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	NN<Text::String> s;
	if (!this->GetNewStr(colIndex).SetTo(s))
	{
		return nullptr;
	}
	UIntOS byteConv;
	UnsafeArray<WChar> ret = Text::StrUTF8_WCharC(buff, s->v, s->leng, byteConv);
	s->Release();
	return ret;
}

Bool DB::SQL::SQLFileDBReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return false;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::String:
		sb->Append(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue());
		return true;
	case SQLValue::ValueType::I32:
		sb->AppendI32(NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue());
		return true;
	case SQLValue::ValueType::I64:
		sb->AppendI64(NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue());
		return true;
	case SQLValue::ValueType::F64:
		sb->AppendDouble(NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue());
		return true;
	case SQLValue::ValueType::Bool:
		sb->Append(NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue() ? CSTR("true") : CSTR("false"));
		return true;
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return false;
	}
}

Optional<Text::String> DB::SQL::SQLFileDBReader::GetNewStr(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return nullptr;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::String:
		return NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->Clone();
	case SQLValue::ValueType::I32:
		sptr = Text::StrInt32(sbuff, NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue());
		return Text::String::NewP(sbuff, sptr);
	case SQLValue::ValueType::I64:
		sptr = Text::StrInt64(sbuff, NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue());
		return Text::String::NewP(sbuff, sptr);
	case SQLValue::ValueType::F64:
		sptr = Text::StrDouble(sbuff, NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue());
		return Text::String::NewP(sbuff, sptr);
	case SQLValue::ValueType::Bool:
		return Text::String::New(NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue() ? CSTR("true") : CSTR("false"));
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return nullptr;
	}
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLFileDBReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	NN<Text::String> s;
	if (!this->GetNewStr(colIndex).SetTo(s))
	{
		return nullptr;
	}
	UnsafeArray<UTF8Char> ret = Text::StrConcatCS(buff, s->v, s->leng, buffSize);
	s->Release();
	return ret;
}

Data::Timestamp DB::SQL::SQLFileDBReader::GetTimestamp(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return Data::Timestamp(nullptr);
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::String:
		return Data::Timestamp::FromStr(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->ToCString(), this->tzQhr);
	case SQLValue::ValueType::I32:
	case SQLValue::ValueType::I64:
	case SQLValue::ValueType::F64:
	case SQLValue::ValueType::Bool:
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return Data::Timestamp(nullptr);
	}
}

Double DB::SQL::SQLFileDBReader::GetDblOrNAN(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return NAN;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::I32:
		return (Double)NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::I64:
		return (Double)NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::F64:
		return NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::Bool:
		return NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue() ? 1.0 : 0.0;
	case SQLValue::ValueType::String:
		return Text::StrToDoubleOrNAN(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->v);
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return NAN;
	}
}

Bool DB::SQL::SQLFileDBReader::GetBool(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return false;
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::Bool:
		return NN<SQLValueBool>::ConvertFrom(sqlValue)->GetValue();
	case SQLValue::ValueType::I32:
		return NN<SQLValueI32>::ConvertFrom(sqlValue)->GetValue() != 0;
	case SQLValue::ValueType::I64:
		return NN<SQLValueI64>::ConvertFrom(sqlValue)->GetValue() != 0;
	case SQLValue::ValueType::F64:
		return NN<SQLValueF64>::ConvertFrom(sqlValue)->GetValue() != 0;
	case SQLValue::ValueType::String:
		return Text::StrToBool(UnsafeArrayOpt<const UTF8Char>(NN<SQLValueString>::ConvertFrom(sqlValue)->GetValue()->v));
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		return false;
	}
}

UIntOS DB::SQL::SQLFileDBReader::GetBinarySize(UIntOS colIndex)
{
	(void)colIndex;
	return 0;
}

UIntOS DB::SQL::SQLFileDBReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	(void)colIndex;
	(void)buff;
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::SQL::SQLFileDBReader::GetVector(UIntOS colIndex)
{
	(void)colIndex;
	return nullptr;
}

Bool DB::SQL::SQLFileDBReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	(void)colIndex;
	(void)uuid;
	return false;
}

Bool DB::SQL::SQLFileDBReader::IsNull(UIntOS colIndex)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
		return true;
	return sqlValue->GetValueType() == SQLValue::ValueType::Null;
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLFileDBReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	NN<SQLInsertCommand> insertCmd;
	if (!this->firstInsertCmd.SetTo(insertCmd) || colIndex >= this->colIndexMap.GetCount())
	{
		return nullptr;
	}
	Optional<Text::String> optCol = insertCmd->GetColumn(this->colIndexMap.GetItem(colIndex));
	NN<Text::String> colName;
	if (!optCol.SetTo(colName))
	{
		return nullptr;
	}
	return colName->ConcatTo(buff);
}

DB::DBUtil::ColType DB::SQL::SQLFileDBReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	NN<SQLValue> sqlValue;
	if (!this->GetSQLValue(colIndex).SetTo(sqlValue))
	{
		colSize.Set(0);
		return DB::DBUtil::CT_Unknown;
	}
	switch (sqlValue->GetValueType())
	{
	case SQLValue::ValueType::String:
		colSize.Set(0x7fffffff);
		return DB::DBUtil::CT_VarUTF8Char;
	case SQLValue::ValueType::I32:
		colSize.Set(11);
		return DB::DBUtil::CT_Int32;
	case SQLValue::ValueType::I64:
		colSize.Set(21);
		return DB::DBUtil::CT_Int64;
	case SQLValue::ValueType::F64:
		colSize.Set(50);
		return DB::DBUtil::CT_Double;
	case SQLValue::ValueType::Bool:
		colSize.Set(5);
		return DB::DBUtil::CT_Bool;
	case SQLValue::ValueType::Function:
	case SQLValue::ValueType::ObjectPath:
	case SQLValue::ValueType::Null:
	default:
		colSize.Set(0);
		return DB::DBUtil::CT_Unknown;
	}
}

Bool DB::SQL::SQLFileDBReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	UTF8Char sbuff[256];
	UnsafeArrayOpt<UTF8Char> sptr = this->GetName(colIndex, sbuff);
	if (sptr.IsNull())
	{
		return false;
	}
	UIntOS colSize;
	DB::DBUtil::ColType colType = this->GetColType(colIndex, colSize);
	colDef->SetColName(CSTRP(sbuff, sptr.Or(sbuff)));
	colDef->SetColType(colType);
	colDef->SetColSize(colSize);
	colDef->SetColDP(colType == DB::DBUtil::CT_Double ? 20 : 0);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 1, 1);
	colDef->SetDefVal(Text::CString(nullptr));
	colDef->SetAttr(Text::CString(nullptr));
	return true;
}
