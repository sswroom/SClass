#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/DBClassBuilder.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

Bool DB::DBReader::GetVariItem(UOSInt colIndex, Data::VariItem *item)
{
	if (colIndex >= this->ColCount())
	{
		return false;
	}
	if (this->IsNull(colIndex))
	{
		item->SetNull();
		return true;
	}
	UOSInt size;
	switch (this->GetColType(colIndex, &size))
	{
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_NChar:
		{
			Text::String *s;
			item->SetStr(s = this->GetNewStr(colIndex));
			s->Release();
		}
		return true;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
	{
		Data::DateTime dt;
		this->GetDate(colIndex, &dt);
		item->SetDate(&dt);
		return true;
	}
	case DB::DBUtil::CT_Double:
		item->SetF64(this->GetDbl(colIndex));
		return true;
	case DB::DBUtil::CT_Float:
		item->SetF32((Single)this->GetDbl(colIndex));
		return true;
	case DB::DBUtil::CT_Bool:
		item->SetBool(this->GetBool(colIndex));
		return true;
	case DB::DBUtil::CT_Byte:
		item->SetU8((UInt8)this->GetInt32(colIndex));
		return true;
	case DB::DBUtil::CT_Int16:
		item->SetI16((Int16)this->GetInt32(colIndex));
		return true;
	case DB::DBUtil::CT_UInt16:
		item->SetU16((UInt16)this->GetInt32(colIndex));
		return true;
	case DB::DBUtil::CT_Int32:
		item->SetI32(this->GetInt32(colIndex));
		return true;
	case DB::DBUtil::CT_UInt32:
		item->SetU32((UInt32)this->GetInt32(colIndex));
		return true;
	case DB::DBUtil::CT_Int64:
		item->SetI64(this->GetInt64(colIndex));
		return true;
	case DB::DBUtil::CT_UInt64:
		item->SetU64((UInt64)this->GetInt64(colIndex));
		return true;
	case DB::DBUtil::CT_Binary:
		{
			size = this->GetBinarySize(colIndex);
			UInt8 *binBuff = MemAlloc(UInt8, size);
			this->GetBinary(colIndex, binBuff);
			item->SetByteArr(binBuff, size);
			MemFree(binBuff);
			return true;
		}
		break;
	case DB::DBUtil::CT_Vector:
		item->SetVectorDirect(this->GetVector(colIndex));
		return true;
	case DB::DBUtil::CT_UUID:
		{
			Data::UUID *uuid;
			NEW_CLASS(uuid, Data::UUID());
			this->GetUUID(colIndex, uuid);
			item->SetUUIDDirect(uuid);
			return true;
		}
	case DB::DBUtil::CT_Unknown:
	default:
		return false;
	}
}

DB::TableDef *DB::DBReader::GenTableDef(Text::CString tableName)
{
	DB::TableDef *table;
	DB::ColDef *colDef;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(table, DB::TableDef(tableName));
	i = 0;
	j = this->ColCount();
	while (i < j)
	{
		NEW_CLASS(colDef, DB::ColDef(CSTR("")));
		this->GetColDef(i, colDef);
		table->AddCol(colDef);
		i++;
	}
	return table;
}

Data::VariObject *DB::DBReader::CreateVariObject()
{
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	UOSInt size;
	Text::String *s;
	DB::DBUtil::ColType ctype;
	Data::VariObject *obj;
	Data::DateTime dt;
	UInt8 *binBuff;
	NEW_CLASS(obj, Data::VariObject(Data::VariObject::NameType::Database));
	i = 0;
	j = this->ColCount();
	while (i < j)
	{
		this->GetName(i, sbuff);
		if (this->IsNull(i))
		{
			obj->SetItemNull(sbuff);
		}
		else
		{
			ctype = this->GetColType(i, &size);
			switch (ctype)
			{
			case DB::DBUtil::CT_VarChar:
			case DB::DBUtil::CT_Char:
			case DB::DBUtil::CT_NVarChar:
			case DB::DBUtil::CT_NChar:
				obj->SetItemStr(sbuff, s = this->GetNewStr(i));
				s->Release();
				break;
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_DateTime2:
				this->GetDate(i, &dt);
				obj->SetItemDate(sbuff, &dt);
				break;
			case DB::DBUtil::CT_Double:
				obj->SetItemF64(sbuff, this->GetDbl(i));
				break;
			case DB::DBUtil::CT_Float:
				obj->SetItemF32(sbuff, (Single)this->GetDbl(i));
				break;
			case DB::DBUtil::CT_Bool:
				obj->SetItemBool(sbuff, this->GetBool(i));
				break;
			case DB::DBUtil::CT_Byte:
				obj->SetItemU8(sbuff, (UInt8)this->GetInt32(i));
				break;
			case DB::DBUtil::CT_Int16:
				obj->SetItemI16(sbuff, (Int16)this->GetInt32(i));
				break;
			case DB::DBUtil::CT_UInt16:
				obj->SetItemU16(sbuff, (UInt16)this->GetInt32(i));
				break;
			case DB::DBUtil::CT_Int32:
				obj->SetItemI32(sbuff, this->GetInt32(i));
				break;
			case DB::DBUtil::CT_UInt32:
				obj->SetItemU32(sbuff, (UInt32)this->GetInt32(i));
				break;
			case DB::DBUtil::CT_Int64:
				obj->SetItemI64(sbuff, this->GetInt64(i));
				break;
			case DB::DBUtil::CT_UInt64:
				obj->SetItemU64(sbuff, (UInt64)this->GetInt64(i));
				break;
			case DB::DBUtil::CT_Binary:
				size = this->GetBinarySize(i);
				binBuff = MemAlloc(UInt8, size);
				this->GetBinary(i, binBuff);
				obj->SetItemByteArray(sbuff, binBuff, size);
				MemFree(binBuff);
				break;
			case DB::DBUtil::CT_Vector:
				obj->SetItemVectorDirect(sbuff, this->GetVector(i));
				break;
			case DB::DBUtil::CT_UUID:
				{
					Data::UUID uuid;
					this->GetUUID(i, &uuid);
					obj->SetItemUUID(sbuff, &uuid);
				}
				break;
			case DB::DBUtil::CT_Unknown:
			default:
				break;
			}
		}
		i++;
	}
	return obj;
}

Data::Class *DB::DBReader::CreateClass()
{
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	UOSInt size;
	DB::DBClassBuilder builder;
	i = 0;
	j = this->ColCount();
	while (i < j)
	{
		this->GetName(i, sbuff);
		builder.AddItem(sbuff, this->GetColType(i, &size));
		i++;
	}
	return builder.GetResultClass();
}