#ifndef _SM_DB_COLDEF
#define _SM_DB_COLDEF
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace DB
{
	class ColDef
	{
	public:
		enum class AutoIncType
		{
			None,
			Default,
			Always
		};

		enum class GeometryType //colSize if colType = CT_Vector
		{
			Unknown,
			Any,
			AnyZ,
			AnyZM,
			AnyM,
			Point,
			PointZ,
			PointZM,
			PointM,
			Multipoint,
			MultipointZ,
			MultipointZM,
			MultipointM,
			Polyline,
			PolylineZ,
			PolylineZM,
			PolylineM,
			Polygon,
			PolygonZ,
			PolygonZM,
			PolygonM,
			Rectangle,
			RectangleZ,
			RectangleZM,
			RectangleM,
			Path,
			PathZ,
			PathZM,
			PathM,
			MultiPolygon,
			MultiPolygonZ,
			MultiPolygonZM,
			MultiPolygonM
		};
	private:
		NotNullPtr<Text::String> colName;
		DB::DBUtil::ColType colType;
		Optional<Text::String> nativeType;
		UOSInt colSize;
		UOSInt colDP;
		Bool notNull;
		Bool pk;
		AutoIncType autoInc;
		Int64 autoIncStartIndex;
		Int64 autoIncStep;
		Optional<Text::String> defVal;
		Optional<Text::String> attr;

		static void AppendDefVal(NotNullPtr<DB::SQLBuilder> sql, Text::CStringNN defVal, UOSInt colSize);
	public:
		ColDef(Text::CString colName);
		ColDef(NotNullPtr<Text::String> colName);
		~ColDef();

		NotNullPtr<Text::String> GetColName() const;
		DB::DBUtil::ColType GetColType() const;
		Optional<Text::String> GetNativeType() const;
		UOSInt GetColSize() const;
		UOSInt GetColDP() const;
		Bool IsNotNull() const;
		Bool IsPK() const;
		Bool IsAutoInc() const;
		AutoIncType GetAutoIncType() const;
		Int64 GetAutoIncStartIndex() const;
		Int64 GetAutoIncStep() const;
		Optional<Text::String> GetDefVal() const;
		Optional<Text::String> GetAttr() const;
		Bool GetDefVal(NotNullPtr<DB::SQLBuilder> sql) const;
		DB::ColDef::GeometryType GetGeometryType() const;
		UInt32 GetGeometrySRID() const;

		void SetColName(NotNullPtr<const UTF8Char> colName);
		void SetColName(Text::CString colName);
		void SetColName(NotNullPtr<Text::String> colName);
		void SetColType(DB::DBUtil::ColType colType);
		void SetNativeType(Optional<Text::String> nativeType);
		void SetNativeType(Text::CString nativeType);
		void SetColSize(UOSInt colSize);
		void SetColDP(UOSInt colDP);
		void SetNotNull(Bool notNull);
		void SetPK(Bool pk);
		void SetAutoIncNone();
		void SetAutoInc(AutoIncType autoInc, Int64 startIndex, Int64 incStep);
		void SetDefVal(Text::CString defVal);
		void SetDefVal(Optional<Text::String> defVal);
		void SetAttr(Text::CString attr);
		void SetAttr(Optional<Text::String> attr);
		void SetGeometrySRID(UInt32 srid);

		void Set(NotNullPtr<const ColDef> colDef);

		UTF8Char *ToColTypeStr(UTF8Char *sbuff) const;
		NotNullPtr<ColDef> Clone() const;

		static GeometryType GeometryTypeAdjust(GeometryType geomType, Bool hasZ, Bool hasM);
		static Text::CStringNN GeometryTypeGetName(GeometryType geomType);
	};
}
#endif
