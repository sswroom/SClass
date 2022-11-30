#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "DB/DBRow.h"
#include "Math/WKTWriter.h"

void DB::DBRow::FreeField(DB::DBRow::Field *field)
{
	DataType dtype = GetDataType(field);
	if (dtype == DT_STRING)
	{
		SDEL_TEXT(field->committedData.str);
		SDEL_TEXT(field->currentData.str);
	}
	else if (dtype == DT_VECTOR)
	{
		SDEL_CLASS(field->committedData.vec);
		SDEL_CLASS(field->currentData.vec);
	}
	else if (dtype == DT_BINARY)
	{
		SMEMFREE(field->committedData.bin);
		SMEMFREE(field->currentData.bin);
	}
	MemFree(field);
}

DB::DBRow::DataType DB::DBRow::GetDataType(DB::DBRow::Field *field) const
{
	switch (field->def->GetColType())
	{
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		return DT_DATETIME;
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return DT_STRING;
	case DB::DBUtil::CT_Bool:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_UInt64:
		return DT_INT64;
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Double:
		return DT_DOUBLE;
	case DB::DBUtil::CT_Vector:
		return DT_VECTOR;
	case DB::DBUtil::CT_Binary:
		return DT_BINARY;
	case DB::DBUtil::CT_Unknown:
		return DT_UNKNOWN;
	}
	return DT_UNKNOWN;
}

Bool DB::DBRow::SetFieldNull(DB::DBRow::Field *field)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	switch (dtype)
	{
	case DT_STRING:
		SDEL_TEXT(field->currentData.str);
		break;
	case DT_VECTOR:
		SDEL_CLASS(field->currentData.vec);
		break;
	case DT_BINARY:
		SMEMFREE(field->currentData.bin);
		break;
	case DT_DATETIME:
	case DT_INT64:
	case DT_DOUBLE:
	case DT_UNKNOWN:
		break;
	}
	field->currentChanged = true;
	field->currentNull = true;
	return true;
}

Bool DB::DBRow::SetFieldStr(DB::DBRow::Field *field, const UTF8Char *strValue)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_STRING)
	{
		return false;
	}
	SDEL_TEXT(field->currentData.str);
	field->currentData.str = SCOPY_TEXT(strValue);
	field->currentChanged = true;
	field->currentNull = (field->currentData.str == 0);
	return true;
}

Bool DB::DBRow::SetFieldInt64(DB::DBRow::Field *field, Int64 intValue)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_INT64)
	{
		return false;
	}
	field->currentData.iVal = intValue;
	field->currentChanged = true;
	field->currentNull = false;
	return true;
}

Bool DB::DBRow::SetFieldDouble(DB::DBRow::Field *field, Double dblValue)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_DOUBLE)
	{
		return false;
	}
	field->currentData.dVal = dblValue;
	field->currentChanged = true;
	field->currentNull = false;
	return true;
}

Bool DB::DBRow::SetFieldDate(DB::DBRow::Field *field, Data::Timestamp ts)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_DATETIME)
	{
		return false;
	}
	field->currentChanged = true;
	if (ts.IsNull())
	{
		field->currentNull = true;
	}
	else
	{
		field->currentData.ts = ts;
		field->currentNull = false;
	}
	return true;
}

Bool DB::DBRow::SetFieldVector(DB::DBRow::Field *field, Math::Geometry::Vector2D *vec)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_VECTOR)
	{
		return false;
	}
	SDEL_CLASS(field->currentData.vec);
	field->currentChanged = true;
	if (vec == 0)
	{
		field->currentNull = true;
	}
	else
	{
		field->currentData.vec = vec->Clone();
		field->currentNull = false;
	}
	return true;
}

Bool DB::DBRow::SetFieldBinary(DB::DBRow::Field *field, const UInt8 *buff, UOSInt buffSize)
{
	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_BINARY)
	{
		return false;
	}
	SMEMFREE(field->currentData.bin);
	field->currentChanged = true;
	if (buff == 0)
	{
		field->currentNull = true;
	}
	else
	{
		field->currentData.bin = MemAlloc(UInt8, buffSize + 4);
		WriteUInt32(field->currentData.bin, (UInt32)buffSize);
		MemCopyNO(&field->currentData.bin[4], buff, buffSize);
		field->currentNull = false;
	}
	return true;
}

Bool DB::DBRow::IsFieldNull(DB::DBRow::Field *field) const
{
	if (field->currentChanged)
	{
		return field->currentNull;
	}
	else
	{
		return field->committedNull;
	}
}

const UTF8Char *DB::DBRow::GetFieldStr(DB::DBRow::Field *field) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_STRING)
	{
		return 0;
	}
	if (field->currentChanged)
	{
		return field->currentData.str;
	}
	else
	{
		return field->committedData.str;
	}	
}

Int64 DB::DBRow::GetFieldInt64(DB::DBRow::Field *field) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_INT64)
	{
		return 0;
	}
	if (field->currentChanged)
	{
		return field->currentData.iVal;
	}
	else
	{
		return field->committedData.iVal;
	}
}

Double DB::DBRow::GetFieldDouble(DB::DBRow::Field *field) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_DOUBLE)
	{
		return 0;
	}
	if (field->currentChanged)
	{
		return field->currentData.dVal;
	}
	else
	{
		return field->committedData.dVal;
	}
}

Data::Timestamp DB::DBRow::GetFieldDate(DB::DBRow::Field *field) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_DATETIME)
	{
		return Data::Timestamp(0);
	}
	if (field->currentChanged)
	{
		return field->currentData.ts;
	}
	else
	{
		return field->committedData.ts;
	}
}

Math::Geometry::Vector2D *DB::DBRow::GetFieldVector(DB::DBRow::Field *field) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_VECTOR)
	{
		return 0;
	}
	if (field->currentChanged)
	{
		return field->currentData.vec;
	}
	else
	{
		return field->committedData.vec;
	}	
}

const UInt8 *DB::DBRow::GetFieldBinary(DB::DBRow::Field *field, UOSInt *buffSize) const
{
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_BINARY)
	{
		return 0;
	}
	const UInt8 *binBuff;
	if (field->currentChanged)
	{
		binBuff = field->currentData.bin;
	}
	else
	{
		binBuff = field->committedData.bin;
	}
	
	if (binBuff)
	{
		*buffSize = ReadUInt32(binBuff);
		return binBuff + 4;
	}
	else
	{
		return 0;
	}
}


DB::DBRow::DBRow(TableDef *table)
{
	this->table = table;
	DB::ColDef *col;
	DB::DBRow::Field *field;
	UOSInt i = 0;
	UOSInt j = table->GetColCnt();
	while (i < j)
	{
		col = table->GetCol(i);
		field = MemAlloc(DB::DBRow::Field, 1);
		field->def = col;
		field->currentNull = true;
		field->currentData.iVal = 0;
		field->currentChanged = false;
		field->committedNull = true;
		field->committedData.iVal = 0;
		this->dataMap.Put(field->def->GetColName()->v, field);
		i++;
	}
}

DB::DBRow::~DBRow()
{
	const Data::ArrayList<Field*> *fieldList = this->dataMap.GetValues();
	LIST_CALL_FUNC(fieldList, this->FreeField);
}

Bool DB::DBRow::SetByReader(DB::DBReader *r, Bool commit)
{
	DB::ColDef *col;
	DB::DBRow::Field *field;
	UOSInt i = 0;
	UOSInt j = this->table->GetColCnt();
	while (i < j)
	{
		col = this->table->GetCol(i);
		field = this->dataMap.Get(col->GetColName()->v);
		if (field == 0)
		{
			return false;
		}
		if (r->IsNull(i))
		{
			this->SetFieldNull(field);
		}
		else
		{
			DB::DBRow::DataType dtype = this->GetDataType(field);
			switch (dtype)
			{
			case DT_BINARY:
				{
					UOSInt size = r->GetBinarySize(i);
					UInt8 *buff = MemAlloc(UInt8, size);
					r->GetBinary(i, buff);
					this->SetFieldBinary(field, buff, size);
					MemFree(buff);
				}
				break;
			case DT_VECTOR:
				{
					Math::Geometry::Vector2D *vec = r->GetVector(i);
					this->SetFieldVector(field, vec);
					SDEL_CLASS(vec);
				}
				break;
			case DT_INT64:
				this->SetFieldInt64(field, r->GetInt64(i));
				break;
			case DT_STRING:
				{
					Text::String *s = r->GetNewStr(i);
					this->SetFieldStr(field, s->v);
					s->Release();
				}
				break;
			case DT_DOUBLE:
				this->SetFieldDouble(field, r->GetDbl(i));
				break;
			case DT_DATETIME:
				this->SetFieldDate(field, r->GetTimestamp(i));
				break;
			case DT_UNKNOWN:
				break;
			}
		}
		i++;
	}
	if (commit)
	{
		this->Commit();
	}
	return true;
}

DB::ColDef *DB::DBRow::GetFieldType(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field)
	{
		return field->def;
	}
	else
	{
		return 0;
	}
}

DB::DBRow::DataType DB::DBRow::GetFieldDataType(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field)
	{
		return this->GetDataType(field);
	}
	return DT_UNKNOWN;
}

Bool DB::DBRow::SetValueNull(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldNull(field);
}

Bool DB::DBRow::SetValueStr(const UTF8Char *fieldName, const UTF8Char *strValue)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldStr(field, strValue);
}

Bool DB::DBRow::SetValueInt64(const UTF8Char *fieldName, Int64 intValue)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldInt64(field, intValue);
}

Bool DB::DBRow::SetValueDouble(const UTF8Char *fieldName, Double dblValue)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldDouble(field, dblValue);
}

Bool DB::DBRow::SetValueDate(const UTF8Char *fieldName, Data::Timestamp ts)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldDate(field, ts);
}

Bool DB::DBRow::SetValueVector(const UTF8Char *fieldName, Math::Geometry::Vector2D *vec)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldVector(field, vec);
}

Bool DB::DBRow::SetValueBinary(const UTF8Char *fieldName, const UInt8 *buff, UOSInt buffSize)
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return false;
	}
	return this->SetFieldBinary(field, buff, buffSize);
}

Bool DB::DBRow::IsNull(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return true;
	}
	return this->IsFieldNull(field);
}

const UTF8Char *DB::DBRow::GetValueStr(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	return this->GetFieldStr(field);
}

Int64 DB::DBRow::GetValueInt64(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	return this->GetFieldInt64(field);
}

Double DB::DBRow::GetValueDouble(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	return this->GetFieldDouble(field);
}

Data::Timestamp DB::DBRow::GetValueDate(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return Data::Timestamp(0);
	}
	return this->GetFieldDate(field);
}

Math::Geometry::Vector2D *DB::DBRow::GetValueVector(const UTF8Char *fieldName) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	return this->GetFieldVector(field);
}

const UInt8 *DB::DBRow::GetValueBinary(const UTF8Char *fieldName, UOSInt *buffSize) const
{
	DB::DBRow::Field *field = this->dataMap.Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	return this->GetFieldBinary(field, buffSize);
}

void DB::DBRow::Commit()
{
	const Data::ArrayList<DB::DBRow::Field*> *fieldList = this->dataMap.GetValues();
	DB::DBRow::Field *field;
	UOSInt i = fieldList->GetCount();
	while (i-- > 0)
	{
		field = fieldList->GetItem(i);
		if (field->currentChanged)
		{
			DataType dtype = this->GetDataType(field);
			switch (dtype)
			{
			case DT_STRING:
				SDEL_TEXT(field->committedData.str);
				field->committedData.str = field->currentData.str;
				field->currentData.str = 0;
				break;
			case DT_VECTOR:
				SDEL_CLASS(field->committedData.vec);
				field->committedData.vec = field->currentData.vec;
				field->currentData.vec = 0;
				break;
			case DT_DATETIME:
				field->committedData.ts = field->currentData.ts;
				field->currentData.ts.inst = Data::TimeInstant(0, 0);
				break;
			case DT_INT64:
				field->committedData.iVal = field->currentData.iVal;
				field->currentData.iVal = 0;
				break;
			case DT_DOUBLE:
				field->committedData.dVal = field->currentData.dVal;
				field->currentData.dVal = 0;
				break;
			case DT_BINARY:
				SMEMFREE(field->committedData.bin);
				field->committedData.bin = field->currentData.bin;
				field->currentData.bin = 0;
				break;
			case DT_UNKNOWN:
				field->committedData.iVal = field->currentData.iVal;
				field->currentData.iVal = 0;
				break;
			}

			field->committedNull = field->currentNull;
			field->currentNull = true;
			field->currentChanged = false;
		}
	}
}

void DB::DBRow::Rollback()
{
	const Data::ArrayList<DB::DBRow::Field*> *fieldList = this->dataMap.GetValues();
	DB::DBRow::Field *field;
	UOSInt i = fieldList->GetCount();
	while (i-- > 0)
	{
		field = fieldList->GetItem(i);
		if (field->currentChanged)
		{
			DataType dtype = this->GetDataType(field);
			switch (dtype)
			{
			case DT_STRING:
				SDEL_TEXT(field->currentData.str);
				break;
			case DT_VECTOR:
				SDEL_CLASS(field->currentData.vec);
				break;
			case DT_DATETIME:
				field->currentData.ts.inst = Data::TimeInstant(0, 0);
				break;
			case DT_BINARY:
				SMEMFREE(field->currentData.bin);
				break;
			case DT_INT64:
				field->currentData.iVal = 0;
				break;
			case DT_DOUBLE:
				field->currentData.dVal = 0;
				break;
			case DT_UNKNOWN:
				field->currentData.iVal = 0;
				break;
			}

			field->currentNull = true;;
			field->currentChanged = false;
		}
	}
}

Bool DB::DBRow::GetSinglePKI64(Int64 *key) const
{
	Bool hasKey = false;
	const Data::ArrayList<DB::DBRow::Field*> *fieldList = this->dataMap.GetValues();
	DB::DBRow::Field *field;
	UOSInt i = fieldList->GetCount();
	while (i-- > 0)
	{
		field = fieldList->GetItem(i);
		if (field->def->IsPK())
		{
			if (hasKey)
			{
				return false;
			}
			else if (this->GetDataType(field) != DT_INT64)
			{
				return false;
			}
			else if (this->IsFieldNull(field))
			{
				return false;
			}
			*key = this->GetFieldInt64(field);
			hasKey = true;
		}
	}
	return hasKey;
}

void DB::DBRow::ToString(Text::StringBuilderUTF8 *sb) const
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	DB::ColDef *col;
	DB::DBRow::Field *field;
	const UInt8 *buff;
	Math::WKTWriter wkt;
	Math::Geometry::Vector2D *vec;
	DataType dtype;
	UOSInt i = 0;
	UOSInt j = this->table->GetColCnt();
	UOSInt k;
	UOSInt strLen;
	this->AppendTableName(sb);
	sb->AppendUTF8Char('[');
	while (i < j)
	{
		col = this->table->GetCol(i);
		field = this->dataMap.Get(col->GetColName()->v);
		if (field)
		{
			if (i > 0)
			{
				sb->AppendC(UTF8STRC(", "));
			}
			this->AppendVarNameForm(sb, col->GetColName()->v);
			sb->AppendUTF8Char('=');
			dtype = this->GetDataType(field);
			if (this->IsFieldNull(field))
			{
				sb->AppendC(UTF8STRC("null"));
			}
			else
			{
				switch (dtype)
				{
				case DT_DATETIME:
					sptr = this->GetFieldDate(field).ToString(sbuff);
					sb->AppendUTF8Char('\"');
					sb->AppendP(sbuff, sptr);
					sb->AppendUTF8Char('\"');
					break;
				case DT_BINARY:
					k = 0;
					buff = this->GetFieldBinary(field, &k);
					strLen = DB::DBUtil::SDBBinLeng(buff, k, table->GetSQLType());
					if (strLen < sizeof(sbuff) - 1)
					{
						sptr = DB::DBUtil::SDBBin(sbuff, buff, k, table->GetSQLType());
						sb->AppendP(sbuff, sptr);
					}
					else
					{
						UTF8Char *tmpBuff = MemAlloc(UTF8Char, strLen + 1);
						sptr = DB::DBUtil::SDBBin(tmpBuff, buff, k, table->GetSQLType());
						sb->AppendP(tmpBuff, sptr);
						MemFree(tmpBuff);
					}
					break;
				case DT_DOUBLE:
					sptr = DB::DBUtil::SDBDbl(sbuff, this->GetFieldDouble(field), table->GetSQLType());
					sb->AppendP(sbuff, sptr);
					break;
				case DT_INT64:
					sptr = DB::DBUtil::SDBInt64(sbuff, this->GetFieldInt64(field), table->GetSQLType());
					sb->AppendP(sbuff, sptr);
					break;
				case DT_STRING:
					buff = this->GetFieldStr(field);
					strLen = DB::DBUtil::SDBStrUTF8Leng(buff, table->GetSQLType());
					if (strLen < sizeof(sbuff) - 1)
					{
						sptr = DB::DBUtil::SDBStrUTF8(sbuff, buff, table->GetSQLType());
						sb->AppendP(sbuff, sptr);
					}
					else
					{
						UTF8Char *tmpBuff = MemAlloc(UTF8Char, strLen + 1);
						sptr = DB::DBUtil::SDBStrUTF8(tmpBuff, buff, table->GetSQLType());
						sb->AppendP(tmpBuff, sptr);
						MemFree(tmpBuff);
					}
					break;
				case DT_VECTOR:
					vec = this->GetFieldVector(field);
					wkt.ToText(sb, vec);
					break;
				case DT_UNKNOWN:
					sb->AppendC(UTF8STRC("?"));
					break;
				}
			}
		}

		i++;
	}
	sb->AppendUTF8Char(']');
}

void DB::DBRow::AppendTableName(Text::StringBuilderUTF8 *sb) const
{
	Text::CString tableName = this->table->GetTableName()->ToCString();
	UOSInt i = tableName.IndexOf('.');
	if (i != INVALID_INDEX)
	{
		tableName = tableName.Substring(i + 1);
	}
	Bool nextCap = true;
	UTF8Char c;
	i = 0;
	while (true)
	{
		c = tableName.v[i];
		if (c == 0)
			break;
		i++;
		if (c == '_')
		{
			nextCap = true;
		}
		else if (nextCap)
		{
			if (c >= 'a' && c <= 'z')
			{
				sb->AppendUTF8Char((UTF8Char)(c - 0x20));
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
			nextCap = false;
		}
		else
		{
			if (c >= 'A' && c <= 'Z')
			{
				sb->AppendUTF8Char((UTF8Char)(c + 0x20));
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
		}
	}
}

void DB::DBRow::AppendVarNameForm(Text::StringBuilderUTF8 *sb, const UTF8Char *colName) const
{
	Bool nextCap = false;
	UTF8Char c;
	while (true)
	{
		c = *colName++;
		if (c == 0)
			break;
		if (c == '_')
		{
			nextCap = true;
		}
		else if (nextCap)
		{
			if (c >= 'a' && c <= 'z')
			{
				sb->AppendUTF8Char((UTF8Char)(c - 0x20));
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
			nextCap = false;
		}
		else
		{
			if (c >= 'A' && c <= 'Z')
			{
				sb->AppendUTF8Char((UTF8Char)(c + 0x20));
			}
			else
			{
				sb->AppendUTF8Char(c);
			}
		}
	}
}

DB::TableDef *DB::DBRow::GetTableDef()
{
	return this->table;
}
