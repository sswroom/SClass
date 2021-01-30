#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "DB/DBRow.h"

void DB::DBRow::FreeField(DB::DBRow::Field *field)
{
	DataType dtype = GetDataType(field);
	if (dtype == DT_DATETIME)
	{
		SDEL_CLASS(field->committedData.dt);
		SDEL_CLASS(field->currentData.dt);
	}
	else if (dtype == DT_STRING)
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

DB::DBRow::DataType DB::DBRow::GetDataType(DB::DBRow::Field *field)
{
	switch (field->def->GetColType())
	{
	case DB::DBUtil::CT_DateTime2:
	case DB::DBUtil::CT_DateTime:
		return DT_DATETIME;
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_VarChar:
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

DB::DBRow::DBRow(TableDef *table)
{
	this->table = table;
	NEW_CLASS(this->dataMap, Data::StringUTF8Map<Field*>());
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
		this->dataMap->Put(field->def->GetColName(), field);
		i++;
	}
}

DB::DBRow::~DBRow()
{
	Data::ArrayList<Field*> *fieldList = this->dataMap->GetValues();
	DEL_LIST_FUNC(fieldList, this->FreeField);
	DEL_CLASS(this->dataMap);
}

DB::ColDef *DB::DBRow::GetFieldType(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field)
	{
		return field->def;
	}
	else
	{
		return 0;
	}
}

DB::DBRow::DataType DB::DBRow::GetFieldDataType(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field)
	{
		return this->GetDataType(field);
	}
	return DT_UNKNOWN;
}

Bool DB::DBRow::SetValueNull(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

	DB::DBRow::DataType dtype = this->GetDataType(field);
	switch (dtype)
	{
	case DT_DATETIME:
		SDEL_CLASS(field->currentData.dt);
		break;
	case DT_STRING:
		SDEL_TEXT(field->currentData.str);
		break;
	case DT_VECTOR:
		SDEL_CLASS(field->currentData.vec);
		break;
	case DT_BINARY:
		SMEMFREE(field->currentData.bin);
		break;
	case DT_INT64:
	case DT_DOUBLE:
	case DT_UNKNOWN:
		break;
	}
	field->currentChanged = true;
	field->currentNull = true;
	return true;
}

Bool DB::DBRow::SetValueStr(const UTF8Char *fieldName, const UTF8Char *strValue)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

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

Bool DB::DBRow::SetValueInt64(const UTF8Char *fieldName, Int64 intValue)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

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

Bool DB::DBRow::SetValueDouble(const UTF8Char *fieldName, Double dblValue)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

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

Bool DB::DBRow::SetValueDate(const UTF8Char *fieldName, Data::DateTime *dt)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

	DB::DBRow::DataType dtype = this->GetDataType(field);
	if (dtype != DT_DATETIME)
	{
		return false;
	}
	field->currentChanged = true;
	if (dt == 0)
	{
		SDEL_CLASS(field->currentData.dt);
		field->currentNull = true;
	}
	else
	{
		if (field->currentData.dt)
		{
			field->currentData.dt->SetValue(dt);
		}
		else
		{
			NEW_CLASS(field->currentData.dt, Data::DateTime(dt));
		}
		field->currentNull = false;
	}
	return true;
}

Bool DB::DBRow::SetValueVector(const UTF8Char *fieldName, Math::Vector2D *vec)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

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

Bool DB::DBRow::SetValueBinary(const UTF8Char *fieldName, const UInt8 *buff, UOSInt buffSize)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return false;
	}

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
		WriteInt32(field->currentData.bin, buffSize);
		MemCopyNO(&field->currentData.bin[4], buff, buffSize);
		field->currentNull = false;
	}
	return true;

}

Bool DB::DBRow::IsNull(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return true;
	}
	if (field->currentChanged)
	{
		return field->currentNull;
	}
	else
	{
		return field->committedNull;
	}
}

const UTF8Char *DB::DBRow::GetValueStr(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
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

Int64 DB::DBRow::GetValueInt64(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
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

Double DB::DBRow::GetValueDouble(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
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

Data::DateTime *DB::DBRow::GetValueDate(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
	DataType dtype = this->GetDataType(field);
	if (dtype != DT_DATETIME)
	{
		return 0;
	}
	if (field->currentChanged)
	{
		return field->currentData.dt;
	}
	else
	{
		return field->committedData.dt;
	}	
}

Math::Vector2D *DB::DBRow::GetValueVector(const UTF8Char *fieldName)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
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

const UInt8 *DB::DBRow::GetValueBinary(const UTF8Char *fieldName, UOSInt *buffSize)
{
	DB::DBRow::Field *field = this->dataMap->Get(fieldName);
	if (field == 0)
	{
		return 0;
	}
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

void DB::DBRow::Commit()
{
	Data::ArrayList<DB::DBRow::Field*> *fieldList = this->dataMap->GetValues();
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
				SDEL_CLASS(field->committedData.dt);
				field->committedData.dt = field->currentData.dt;
				field->currentData.dt = 0;
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
	Data::ArrayList<DB::DBRow::Field*> *fieldList = this->dataMap->GetValues();
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
				SDEL_CLASS(field->currentData.dt);
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
