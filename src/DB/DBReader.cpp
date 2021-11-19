#include "Stdafx.h"
#include "Data/DateTime.h"
#include "DB/DBClassBuilder.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

DB::TableDef *DB::DBReader::GenTableDef(const UTF8Char *tableName)
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
		NEW_CLASS(colDef, DB::ColDef((const UTF8Char*)""));
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
				obj->SetItemStrDirect(sbuff, this->GetNewStr(i));
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