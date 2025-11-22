#ifndef _SM_MAP_ESRI_FILEGDBUTIL
#define _SM_MAP_ESRI_FILEGDBUTIL
#include "Data/ArrayListNN.hpp"
#include "Data/ByteArray.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystem.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		struct FileGDBFieldInfo
		{
			NN<Text::String> name;
			Optional<Text::String> alias;
			UInt8 fieldType;
			UInt32 fieldSize;
			UInt8 flags;
			UInt8 defSize;
			UnsafeArrayOpt<UInt8> defValue;
			UOSInt srsSize;
			UnsafeArrayOpt<UInt8> srsValue;
		};

		struct FileGDBTableInfo
		{
			UOSInt nullableCnt;
			UInt8 geometryType;
			UInt8 tableFlags;
			UInt8 geometryFlags;
			NN<Data::ArrayListNN<FileGDBFieldInfo>> fields;

			Optional<Math::CoordinateSystem> csys;
			Double xOrigin;
			Double yOrigin;
			Double xyScale;
			Double zOrigin;
			Double zScale;
			Double mOrigin;
			Double mScale;
			Double xyTolerance;
			Double zTolerance;
			Double mTolerance;
			Double xMin;
			Double yMin;
			Double xMax;
			Double yMax;
			Double zMin;
			Double zMax;
			Double mMin;
			Double mMax;
			UOSInt spatialGridCnt;
			Double spatialGrid[4];
		};

		class FileGDBUtil
		{
		public:
			static Optional<FileGDBTableInfo> ParseFieldDesc(Data::ByteArray fieldDesc, NN<Math::ArcGISPRJParser> prjParser);
			static void FreeFieldInfo(NN<FileGDBFieldInfo> fieldInfo);
			static void FreeTableInfo(NN<FileGDBTableInfo> tableInfo);
			static NN<FileGDBFieldInfo> FieldInfoClone(NN<FileGDBFieldInfo> tableInfo);
			static NN<FileGDBTableInfo> TableInfoClone(NN<FileGDBTableInfo> tableInfo);
			static UOSInt ReadVarUInt(UnsafeArray<const UInt8> buff, UOSInt ofst, OutParam<UInt64> val);
			static UOSInt ReadVarInt(UnsafeArray<const UInt8> buff, UOSInt ofst, OutParam<Int64> val);
			static UOSInt ReadVarUInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<UInt64> val);
			static UOSInt ReadVarInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<Int64> val);
			static Optional<Math::Geometry::Vector2D> ParseSDERecord(Data::ByteArrayR buff);

			static Text::CStringNN GeometryTypeGetName(UInt8 t);
			static Text::CStringNN FieldTypeGetName(UInt8 t);
		};
	}
}
#endif
