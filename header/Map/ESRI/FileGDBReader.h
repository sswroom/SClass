#ifndef _SM_MAP_ESRI_FILEGDBREADER
#define _SM_MAP_ESRI_FILEGDBREADER
#include "DB/DBReader.h"
#include "DB/QueryConditions.h"
#include "IO/IStreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBReader : public DB::DBReader, public Data::ObjectGetter
		{
		private:
			IO::IStreamData *fd;
			UInt64 currOfst;
			FileGDBTableInfo *tableInfo;
			UOSInt rowSize;
			UInt8 *rowData;
			Int32 objectId;
			UInt8 *fieldNull;
			UInt32 *fieldOfst;
			Data::ArrayList<UOSInt> *columnIndices;
			UOSInt dataOfst;
			UOSInt maxCnt;
			DB::QueryConditions *conditions;

			UOSInt GetFieldIndex(UOSInt colIndex);
		public:
			FileGDBReader(IO::IStreamData *fd, UInt64 ofst, FileGDBTableInfo *tableInfo, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt dataOfst, UOSInt maxCnt, DB::QueryConditions *conditions);
			virtual ~FileGDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged(); //-1 = error
			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
			virtual const UTF8Char *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Vector2D *GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

			virtual Data::VariItem *GetNewItem(const UTF8Char *name);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
			virtual void DelNewStr(const UTF8Char *s);
		};
	}
}
#endif