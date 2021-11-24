#ifndef _SM_MAP_ESRI_FILEGDBUTIL
#define _SM_MAP_ESRI_FILEGDBUTIL
#include "Data/ArrayList.h"
#include "Math/CoordinateSystem.h"

namespace Map
{
	namespace ESRI
	{
		struct FileGDBFieldInfo
		{
			const UTF8Char *name;
			const UTF8Char *alias;
			UInt8 fieldType;
			UInt32 fieldSize;
			UInt8 flags;
			UInt8 defSize;
			UInt8 *defValue;
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
			static FileGDBTableInfo *ParseFieldDesc(const UInt8 *fieldDesc);
			static void FreeFieldInfo(FileGDBFieldInfo *fieldInfo);
			static void FreeTableInfo(FileGDBTableInfo *tableInfo);
			static FileGDBFieldInfo *FieldInfoClone(FileGDBFieldInfo *tableInfo);
			static FileGDBTableInfo *TableInfoClone(FileGDBTableInfo *tableInfo);
			static UOSInt ReadVarUInt(const UInt8 *buff, UOSInt ofst, UInt64 *val);
			static UOSInt ReadVarInt(const UInt8 *buff, UOSInt ofst, Int64 *val);

			static const UTF8Char *GeometryTypeGetName(UInt8 t);
			static const UTF8Char *FieldTypeGetName(UInt8 t);
		};
	}
}
#endif
