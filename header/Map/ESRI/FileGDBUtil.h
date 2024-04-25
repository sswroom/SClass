#ifndef _SM_MAP_ESRI_FILEGDBUTIL
#define _SM_MAP_ESRI_FILEGDBUTIL
#include "Data/ArrayList.h"
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
			Text::String *alias;
			UInt8 fieldType;
			UInt32 fieldSize;
			UInt8 flags;
			UInt8 defSize;
			UInt8 *defValue;
			UOSInt srsSize;
			UInt8 *srsValue;
		};

		struct FileGDBTableInfo
		{
			UOSInt nullableCnt;
			UInt8 geometryType;
			UInt8 tableFlags;
			UInt8 geometryFlags;
			Data::ArrayList<FileGDBFieldInfo*> *fields;

			Math::CoordinateSystem *csys;
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
			static void FreeFieldInfo(FileGDBFieldInfo *fieldInfo);
			static void FreeTableInfo(NN<FileGDBTableInfo> tableInfo);
			static FileGDBFieldInfo *FieldInfoClone(FileGDBFieldInfo *tableInfo);
			static NN<FileGDBTableInfo> TableInfoClone(NN<FileGDBTableInfo> tableInfo);
			static UOSInt ReadVarUInt(const UInt8 *buff, UOSInt ofst, OutParam<UInt64> val);
			static UOSInt ReadVarInt(const UInt8 *buff, UOSInt ofst, OutParam<Int64> val);
			static UOSInt ReadVarUInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<UInt64> val);
			static UOSInt ReadVarInt(Data::ByteArrayR buff, UOSInt ofst, OutParam<Int64> val);
			static Optional<Math::Geometry::Vector2D> ParseSDERecord(Data::ByteArrayR buff);

			static Text::CStringNN GeometryTypeGetName(UInt8 t);
			static Text::CStringNN FieldTypeGetName(UInt8 t);
		};
	}
}
#endif
