#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"

void DB::ColDef::AppendDefVal(DB::SQLBuilder *sql, Text::CString defVal, UOSInt colSize)
{
	if (defVal.StartsWith(UTF8STRC("b'")))
	{
		sql->AppendBool(defVal.Equals(UTF8STRC("b'1'")));
	}
	else if (defVal.Equals(UTF8STRC("CONVERT([datetime2](3),getutcdate())")))
	{
		switch (sql->GetSQLType())
		{
		case DB::SQLType::MSSQL:
			sql->AppendStrC(CSTR("CONVERT([datetime2](3),getutcdate())"));
			break;
		case DB::SQLType::PostgreSQL:
			sql->AppendCmdC(CSTR("(current_timestamp(3) at time zone 'utc')"));
			break;
		case DB::SQLType::MySQL:
			sql->AppendCmdC(CSTR("utc_timestamp("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::SQLType::Access:
		case DB::SQLType::MDBTools:
		case DB::SQLType::Oracle:
		case DB::SQLType::SQLite:
		case DB::SQLType::Unknown:
		case DB::SQLType::WBEM:
		default:
			sql->AppendStrC(CSTR("utcnow(3)"));
			break;
		}
	}
	else if (defVal.Equals(UTF8STRC("getdate()")) || defVal.Equals(UTF8STRC("current_timestamp()")))
	{
		switch (sql->GetSQLType())
		{
		case DB::SQLType::MSSQL:
			sql->AppendStrC(CSTR("getdate()"));
			break;
		case DB::SQLType::MySQL:
			sql->AppendCmdC(CSTR("now("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::SQLType::PostgreSQL:
		case DB::SQLType::Access:
		case DB::SQLType::MDBTools:
		case DB::SQLType::Oracle:
		case DB::SQLType::SQLite:
		case DB::SQLType::Unknown:
		case DB::SQLType::WBEM:
		default:
			sql->AppendStrC(CSTR("now()"));
			break;
		}
	}
	else
	{
		sql->AppendStrC(defVal);
	}
}

DB::ColDef::ColDef(Text::CString colName)
{
	this->colName = Text::String::New(colName);
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = 0;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = AutoIncType::None;
	this->autoIncStartIndex = 1;
	this->autoIncStep = 1;
	this->defVal = 0;
	this->attr = 0;
}

DB::ColDef::ColDef(NotNullPtr<Text::String> colName)
{
	this->colName = colName->Clone();
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = 0;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = AutoIncType::None;
	this->autoIncStartIndex = 1;
	this->autoIncStep = 1;
	this->defVal = 0;
	this->attr = 0;
}

DB::ColDef::~ColDef()
{
	this->colName->Release();
	SDEL_STRING(this->nativeType);
	SDEL_STRING(this->defVal);
	SDEL_STRING(this->attr);
}

NotNullPtr<Text::String> DB::ColDef::GetColName() const
{
	return this->colName;
}

DB::DBUtil::ColType DB::ColDef::GetColType() const
{
	return this->colType;
}

Text::String *DB::ColDef::GetNativeType() const
{
	return this->nativeType;
}

UOSInt DB::ColDef::GetColSize() const
{
	return this->colSize;
}

UOSInt DB::ColDef::GetColDP() const
{
	return this->colDP;
}

Bool DB::ColDef::IsNotNull() const
{
	return this->notNull;
}

Bool DB::ColDef::IsPK() const
{
	return this->pk;
}

Bool DB::ColDef::IsAutoInc() const
{
	return this->autoInc != AutoIncType::None;
}

DB::ColDef::AutoIncType DB::ColDef::GetAutoIncType() const
{
	return this->autoInc;
}

Int64 DB::ColDef::GetAutoIncStartIndex() const
{
	return this->autoIncStartIndex;
}

Int64 DB::ColDef::GetAutoIncStep() const
{
	return this->autoIncStep;
}

Text::String *DB::ColDef::GetDefVal() const
{
	return this->defVal;
}

Text::String *DB::ColDef::GetAttr() const
{
	return this->attr;
}

Bool DB::ColDef::GetDefVal(DB::SQLBuilder *sql) const
{
	if (this->defVal == 0)
		return false;
	if (this->defVal->v[0] == '(' && this->defVal->EndsWith(')'))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->defVal);
		while (sb.v[0] == '(' && sb.EndsWith(')'))
		{
			sb.SetSubstr(1);
			sb.RemoveChars(1);
		}
		AppendDefVal(sql, sb.ToCString(), this->colSize);
	}
	else
	{
		AppendDefVal(sql, this->defVal->ToCString(), this->colSize);
	}
	return true;
}


void DB::ColDef::SetColName(NotNullPtr<const UTF8Char> colName)
{
	this->colName->Release();
	this->colName = Text::String::NewNotNullSlow(colName.Ptr());
}

void DB::ColDef::SetColName(Text::CString colName)
{
	this->colName->Release();
	this->colName = Text::String::New(colName);
}

void DB::ColDef::SetColName(NotNullPtr<Text::String> colName)
{
	this->colName->Release();
	this->colName = colName->Clone();
}
void DB::ColDef::SetColType(DB::DBUtil::ColType colType)
{
	this->colType = colType;
}

void DB::ColDef::SetNativeType(Text::String *nativeType)
{
	SDEL_STRING(this->nativeType);
	this->nativeType = SCOPY_STRING(nativeType);
}

void DB::ColDef::SetNativeType(Text::CString nativeType)
{
	SDEL_STRING(this->nativeType);
	this->nativeType = Text::String::NewOrNull(nativeType);
}

void DB::ColDef::SetColSize(UOSInt colSize)
{
	this->colSize = colSize;
}

void DB::ColDef::SetColDP(UOSInt colDP)
{
	this->colDP = colDP;
}

void DB::ColDef::SetNotNull(Bool notNull)
{
	this->notNull = notNull;
}

void DB::ColDef::SetPK(Bool pk)
{
	this->pk = pk;
}

void DB::ColDef::SetAutoIncNone()
{
	this->autoInc = AutoIncType::None;
	this->autoIncStartIndex = 1;
	this->autoIncStep = 1;
}

void DB::ColDef::SetAutoInc(AutoIncType autoInc, Int64 startIndex, Int64 incStep)
{
	this->autoInc = autoInc;
	this->autoIncStartIndex = startIndex;
	this->autoIncStep = incStep;
}

void DB::ColDef::SetDefVal(Text::CString defVal)
{
	SDEL_STRING(this->defVal);
	this->defVal = Text::String::NewOrNull(defVal);
}

void DB::ColDef::SetDefVal(Text::String *defVal)
{
	SDEL_STRING(this->defVal);
	if (defVal) this->defVal = defVal->Clone().Ptr();
}

void DB::ColDef::SetAttr(Text::CString attr)
{
	SDEL_STRING(this->attr);
	this->attr = Text::String::NewOrNull(attr);
}

void DB::ColDef::SetAttr(Text::String *attr)
{
	SDEL_STRING(this->attr);
	if (attr) this->attr = attr->Clone().Ptr();
}

void DB::ColDef::Set(const ColDef *colDef)
{
	this->SetColName(colDef->colName);
	this->SetColType(colDef->colType);
	this->SetNativeType(colDef->nativeType);
	this->SetColSize(colDef->colSize);
	this->SetColDP(colDef->colDP);
	this->SetNotNull(colDef->notNull);
	this->SetPK(colDef->pk);
	this->SetAutoInc(colDef->autoInc, colDef->autoIncStartIndex, colDef->autoIncStep);
	this->SetDefVal(colDef->defVal);
	this->SetAttr(colDef->attr);
}

UTF8Char *DB::ColDef::ToColTypeStr(UTF8Char *sbuff) const
{
	return DB::DBUtil::ColTypeGetString(sbuff, this->colType, this->colSize, this->colDP);
}

NotNullPtr<DB::ColDef> DB::ColDef::Clone() const
{
	NotNullPtr<DB::ColDef> newObj;
	NEW_CLASSNN(newObj, DB::ColDef(this->colName));
	newObj->Set(this);
	return newObj;
}

DB::ColDef::GeometryType DB::ColDef::GeometryTypeAdjust(GeometryType geomType, Bool hasZ, Bool hasM)
{
	switch (geomType)
	{
	default:
	case GeometryType::Unknown:
	case GeometryType::Any:
	case GeometryType::AnyZ:
	case GeometryType::AnyZM:
	case GeometryType::AnyM:
		if (hasZ)
			if (hasM)
				return GeometryType::AnyZM;
			else
				return GeometryType::AnyZ;
		else
			if (hasM)
				return GeometryType::AnyM;
			else
				return GeometryType::Any;
	case GeometryType::Point:
	case GeometryType::PointZ:
	case GeometryType::PointZM:
	case GeometryType::PointM:
		if (hasZ)
			if (hasM)
				return GeometryType::PointZM;
			else
				return GeometryType::PointZ;
		else
			if (hasM)
				return GeometryType::PointM;
			else
				return GeometryType::Point;
	case GeometryType::Multipoint:
	case GeometryType::MultipointZ:
	case GeometryType::MultipointZM:
	case GeometryType::MultipointM:
		if (hasZ)
			if (hasM)
				return GeometryType::MultipointZM;
			else
				return GeometryType::MultipointZ;
		else
			if (hasM)
				return GeometryType::MultipointM;
			else
				return GeometryType::Multipoint;
	case GeometryType::Polyline:
	case GeometryType::PolylineZ:
	case GeometryType::PolylineZM:
	case GeometryType::PolylineM:
		if (hasZ)
			if (hasM)
				return GeometryType::PolylineZM;
			else
				return GeometryType::PolylineZ;
		else
			if (hasM)
				return GeometryType::PolylineM;
			else
				return GeometryType::Polyline;
	case GeometryType::Polygon:
	case GeometryType::PolygonZ:
	case GeometryType::PolygonZM:
	case GeometryType::PolygonM:
		if (hasZ)
			if (hasM)
				return GeometryType::PolygonZM;
			else
				return GeometryType::PolygonZ;
		else
			if (hasM)
				return GeometryType::PolygonM;
			else
				return GeometryType::Polygon;
	case GeometryType::Rectangle:
	case GeometryType::RectangleZ:
	case GeometryType::RectangleZM:
	case GeometryType::RectangleM:
		if (hasZ)
			if (hasM)
				return GeometryType::RectangleZM;
			else
				return GeometryType::RectangleZ;
		else
			if (hasM)
				return GeometryType::RectangleM;
			else
				return GeometryType::Rectangle;
	case GeometryType::Path:
	case GeometryType::PathZ:
	case GeometryType::PathZM:
	case GeometryType::PathM:
		if (hasZ)
			if (hasM)
				return GeometryType::PathZM;
			else
				return GeometryType::PathZ;
		else
			if (hasM)
				return GeometryType::PathM;
			else
				return GeometryType::Path;
	}
}

Text::CString DB::ColDef::GeometryTypeGetName(GeometryType geomType)
{
	switch (geomType)
	{
	default:
	case GeometryType::Unknown:
		return CSTR("Geometry");
	case GeometryType::Any:
		return CSTR("Geometry");
	case GeometryType::AnyZ:
		return CSTR("GeometryZ");
	case GeometryType::AnyZM:
		return CSTR("GeometryZM");
	case GeometryType::AnyM:
		return CSTR("GeometryM");
	case GeometryType::Point:
		return CSTR("Point");
	case GeometryType::PointZ:
		return CSTR("PointZ");
	case GeometryType::PointZM:
		return CSTR("PointZM");
	case GeometryType::PointM:
		return CSTR("PointM");
	case GeometryType::Multipoint:
		return CSTR("Multipoint");
	case GeometryType::MultipointZ:
		return CSTR("MultipointZ");
	case GeometryType::MultipointZM:
		return CSTR("MultipointZM");
	case GeometryType::MultipointM:
		return CSTR("MultipointM");
	case GeometryType::Polyline:
		return CSTR("Polyline");
	case GeometryType::PolylineZ:
		return CSTR("PolylineZ");
	case GeometryType::PolylineZM:
		return CSTR("PolylineZM");
	case GeometryType::PolylineM:
		return CSTR("PolylineM");
	case GeometryType::Polygon:
		return CSTR("Polygon");
	case GeometryType::PolygonZ:
		return CSTR("PolygonZ");
	case GeometryType::PolygonZM:
		return CSTR("PolygonZM");
	case GeometryType::PolygonM:
		return CSTR("PolygonM");
	case GeometryType::Rectangle:
		return CSTR("Rectangle");
	case GeometryType::RectangleZ:
		return CSTR("RectangleZ");
	case GeometryType::RectangleZM:
		return CSTR("RectangleZM");
	case GeometryType::RectangleM:
		return CSTR("RectangleM");
	case GeometryType::Path:
		return CSTR("Path");
	case GeometryType::PathZ:
		return CSTR("PathZ");
	case GeometryType::PathZM:
		return CSTR("PathZM");
	case GeometryType::PathM:
		return CSTR("PathM");
	}
}
