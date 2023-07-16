#ifndef _SM_MAP_ESRI_FILEGDBUTIL
#define _SM_MAP_ESRI_FILEGDBUTIL
#include "Data/ArrayList.h"
#include "Data/ByteArray.h"
#include "Math/ArcGISPRJParser.h"
#include "Math/CoordinateSystem.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace Map
{
	namespace ESRI
	{
		struct FileGDBFieldInfo
		{
			NotNullPtr<Text::String> name;
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
			static FileGDBTableInfo *ParseFieldDesc(Data::ByteArray fieldDesc, Math::ArcGISPRJParser *prjParser);
			static void FreeFieldInfo(FileGDBFieldInfo *fieldInfo);
			static void FreeTableInfo(FileGDBTableInfo *tableInfo);
			static FileGDBFieldInfo *FieldInfoClone(FileGDBFieldInfo *tableInfo);
			static FileGDBTableInfo *TableInfoClone(FileGDBTableInfo *tableInfo);
			static UOSInt ReadVarUInt(const UInt8 *buff, UOSInt ofst, UInt64 *val);
			static UOSInt ReadVarInt(const UInt8 *buff, UOSInt ofst, Int64 *val);
			static UOSInt ReadVarUInt(Data::ByteArrayR buff, UOSInt ofst, UInt64 *val);
			static UOSInt ReadVarInt(Data::ByteArrayR buff, UOSInt ofst, Int64 *val);

			static Text::CString GeometryTypeGetName(UInt8 t);
			static Text::CString FieldTypeGetName(UInt8 t);
		};
	}
}
#endif
