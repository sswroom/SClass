#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"

void DB::ColDef::AppendDefVal(NN<DB::SQLBuilder> sql, Text::CStringNN defVal, UOSInt colSize)
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

DB::ColDef::ColDef(Text::CStringNN colName)
{
	this->colName = Text::String::New(colName);
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = nullptr;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = AutoIncType::None;
	this->autoIncStartIndex = 1;
	this->autoIncStep = 1;
	this->defVal = nullptr;
	this->attr = nullptr;
}

DB::ColDef::ColDef(NN<Text::String> colName)
{
	this->colName = colName->Clone();
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = nullptr;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = AutoIncType::None;
	this->autoIncStartIndex = 1;
	this->autoIncStep = 1;
	this->defVal = nullptr;
	this->attr = nullptr;
}

DB::ColDef::~ColDef()
{
	this->colName->Release();
	OPTSTR_DEL(this->nativeType);
	OPTSTR_DEL(this->defVal);
	OPTSTR_DEL(this->attr);
}

NN<Text::String> DB::ColDef::GetColName() const
{
	return this->colName;
}

DB::DBUtil::ColType DB::ColDef::GetColType() const
{
	return this->colType;
}

Optional<Text::String> DB::ColDef::GetNativeType() const
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

Optional<Text::String> DB::ColDef::GetDefVal() const
{
	return this->defVal;
}

Optional<Text::String> DB::ColDef::GetAttr() const
{
	return this->attr;
}

Bool DB::ColDef::GetDefVal(NN<DB::SQLBuilder> sql) const
{
	NN<Text::String> s;
	if (!this->defVal.SetTo(s))
		return false;
	if (s->v[0] == '(' && s->EndsWith(')'))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(s);
		while (sb.v[0] == '(' && sb.EndsWith(')'))
		{
			sb.SetSubstr(1);
			sb.RemoveChars(1);
		}
		AppendDefVal(sql, sb.ToCString(), this->colSize);
	}
	else
	{
		AppendDefVal(sql, s->ToCString(), this->colSize);
	}
	return true;
}

DB::ColDef::GeometryType DB::ColDef::GetGeometryType() const
{
	return (DB::ColDef::GeometryType)this->colSize;
}

UInt32 DB::ColDef::GetGeometrySRID() const
{
	return (UInt32)this->colDP;
}

void DB::ColDef::SetColName(UnsafeArray<const UTF8Char> colName)
{
	this->colName->Release();
	this->colName = Text::String::NewNotNullSlow(colName);
}

void DB::ColDef::SetColName(Text::CStringNN colName)
{
	this->colName->Release();
	this->colName = Text::String::New(colName);
}

void DB::ColDef::SetColName(NN<Text::String> colName)
{
	this->colName->Release();
	this->colName = colName->Clone();
}
void DB::ColDef::SetColType(DB::DBUtil::ColType colType)
{
	this->colType = colType;
}

void DB::ColDef::SetNativeType(Optional<Text::String> nativeType)
{
	OPTSTR_DEL(this->nativeType);
	this->nativeType = Text::String::CopyOrNull(nativeType);
}

void DB::ColDef::SetNativeType(Text::CString nativeType)
{
	OPTSTR_DEL(this->nativeType);
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
	OPTSTR_DEL(this->defVal);
	this->defVal = Text::String::NewOrNull(defVal);
}

void DB::ColDef::SetDefVal(Optional<Text::String> defVal)
{
	OPTSTR_DEL(this->defVal);
	this->defVal = Text::String::CopyOrNull(defVal);
}

void DB::ColDef::SetAttr(Text::CString attr)
{
	OPTSTR_DEL(this->attr);
	this->attr = Text::String::NewOrNull(attr);
}

void DB::ColDef::SetAttr(Optional<Text::String> attr)
{
	OPTSTR_DEL(this->attr);
	this->attr = Text::String::CopyOrNull(attr);
}

void DB::ColDef::SetGeometrySRID(UInt32 srid)
{
	this->colDP = srid;
}

void DB::ColDef::Set(NN<const ColDef> colDef)
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

UnsafeArray<UTF8Char> DB::ColDef::ToColTypeStr(UnsafeArray<UTF8Char> sbuff) const
{
	return DB::DBUtil::ColTypeGetString(sbuff, this->colType, this->colSize, this->colDP);
}

NN<DB::ColDef> DB::ColDef::Clone() const
{
	NN<DB::ColDef> newObj;
	NEW_CLASSNN(newObj, DB::ColDef(this->colName));
	newObj->Set(*this);
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
	case GeometryType::MultiPolygon:
	case GeometryType::MultiPolygonZ:
	case GeometryType::MultiPolygonZM:
	case GeometryType::MultiPolygonM:
		if (hasZ)
			if (hasM)
				return GeometryType::MultiPolygonZM;
			else
				return GeometryType::MultiPolygonZ;
		else
			if (hasM)
				return GeometryType::MultiPolygonM;
			else
				return GeometryType::MultiPolygon;
	}
}

Text::CStringNN DB::ColDef::GeometryTypeGetName(GeometryType geomType)
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
	case GeometryType::MultiPolygon:
		return CSTR("MultiPolygon");
	case GeometryType::MultiPolygonZ:
		return CSTR("MultiPolygonZ");
	case GeometryType::MultiPolygonZM:
		return CSTR("MultiPolygonZM");
	case GeometryType::MultiPolygonM:
		return CSTR("MultiPolygonM");
	}
}
