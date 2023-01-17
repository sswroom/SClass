#include "Stdafx.h"
#include "DB/DBClassBuilder.h"

DB::DBClassBuilder::DBClassBuilder()
{
	NEW_CLASS(this->cls, Data::Class(0));
	this->currPos = 0;
}

DB::DBClassBuilder::~DBClassBuilder()
{

}

void DB::DBClassBuilder::AddItem(const UTF8Char *colName, DB::DBUtil::ColType colType)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = DB::DBUtil::DB2FieldName(sbuff, colName);
	switch (colType)
	{
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::Str);
		break;
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::Timestamp);
		break;
	case DB::DBUtil::CT_Decimal:
	case DB::DBUtil::CT_Double:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::F64);
		break;
	case DB::DBUtil::CT_Float:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::F32);
		break;
	case DB::DBUtil::CT_Bool:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::BOOL);
		break;
	case DB::DBUtil::CT_Byte:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::U8);
		break;
	case DB::DBUtil::CT_Int16:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::I16);
		break;
	case DB::DBUtil::CT_UInt16:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::U16);
		break;
	case DB::DBUtil::CT_Int32:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::I32);
		break;
	case DB::DBUtil::CT_UInt32:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::U32);
		break;
	case DB::DBUtil::CT_Int64:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::I64);
		break;
	case DB::DBUtil::CT_UInt64:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::U64);
		break;
	case DB::DBUtil::CT_Binary:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::ByteArr);
		break;
	case DB::DBUtil::CT_Vector:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::Vector);
		break;
	case DB::DBUtil::CT_UUID:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::UUID);
		break;
	case DB::DBUtil::CT_Unknown:
	default:
		currPos += (OSInt)cls->AddField(CSTRP(sbuff, sptr), currPos, Data::VariItem::ItemType::Unknown);
		break;
	}
}

Data::Class *DB::DBClassBuilder::GetResultClass()
{
	return this->cls;
}
