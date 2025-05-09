#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/DBClassBuilder.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

Bool DB::DBReader::GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
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
	switch (this->GetColType(colIndex, size))
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
		{
			Optional<Text::String> s;
			item->SetStr(s = this->GetNewStr(colIndex));
			OPTSTR_DEL(s);
		}
		return true;
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
	{
		item->SetDate(this->GetTimestamp(colIndex));
		return true;
	}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Decimal:
		item->SetF64(this->GetDblOrNAN(colIndex));
		return true;
	case DB::DBUtil::CT_Float:
		item->SetF32((Single)this->GetDblOrNAN(colIndex));
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
		{
			NN<Math::Geometry::Vector2D> vec;
			if (this->GetVector(colIndex).SetTo(vec))
			{
				item->SetVectorDirect(vec);
			}
			else
			{
				item->SetNull();
			}
			return true;
		}
	case DB::DBUtil::CT_UUID:
		{
			NN<Data::UUID> uuid;
			NEW_CLASSNN(uuid, Data::UUID());
			this->GetUUID(colIndex, uuid);
			item->SetUUIDDirect(uuid);
			return true;
		}
	case DB::DBUtil::CT_Unknown:
	default:
		return false;
	}
}

NN<DB::TableDef> DB::DBReader::GenTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::TableDef> table;
	NN<DB::ColDef> colDef;
	UOSInt i;
	UOSInt j;
	NEW_CLASSNN(table, DB::TableDef(schemaName, tableName));
	i = 0;
	j = this->ColCount();
	while (i < j)
	{
		NEW_CLASSNN(colDef, DB::ColDef(CSTR("")));
		this->GetColDef(i, colDef);
		table->AddCol(colDef);
		i++;
	}
	return table;
}

NN<Data::VariObject> DB::DBReader::CreateVariObject()
{
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	UOSInt size;
	Optional<Text::String> ops;
	DB::DBUtil::ColType ctype;
	NN<Data::VariObject> obj;
	Data::DateTime dt;
	UInt8 *binBuff;
	NEW_CLASSNN(obj, Data::VariObject(Data::VariObject::NameType::Database));
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
			ctype = this->GetColType(i, size);
			switch (ctype)
			{
			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
				obj->SetItemStr(sbuff, ops = this->GetNewStr(i));
				OPTSTR_DEL(ops);
				break;
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_Date:
				obj->SetItemTS(sbuff, this->GetTimestamp(i));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Decimal:
				obj->SetItemF64(sbuff, this->GetDblOrNAN(i));
				break;
			case DB::DBUtil::CT_Float:
				obj->SetItemF32(sbuff, (Single)this->GetDblOrNAN(i));
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
					this->GetUUID(i, uuid);
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

NN<Data::Class> DB::DBReader::CreateClass()
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
		builder.AddItem(sbuff, this->GetColType(i, size), false);
		i++;
	}
	return builder.GetResultClass();
}