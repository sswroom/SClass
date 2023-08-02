#ifndef _SM_MAP_ESRI_FILEGDBREADER
#define _SM_MAP_ESRI_FILEGDBREADER
#include "Data/ByteBuffer.h"
#include "Data/QueryConditions.h"
#include "DB/DBReader.h"
#include "IO/StreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBReader : public DB::DBReader, public Data::ObjectGetter
		{
		private:
			NotNullPtr<IO::StreamData> fd;
			UInt64 currOfst;
			FileGDBTableInfo *tableInfo;
			UOSInt rowSize;
			Data::ByteBuffer rowData;
			Int32 objectId;
			UInt8 *fieldNull;
			UInt32 *fieldOfst;
			Data::ArrayList<UOSInt> *columnIndices;
			UOSInt dataOfst;
			UOSInt maxCnt;
			Data::QueryConditions *conditions;

			UOSInt indexCnt;
			UOSInt indexNext;
			Data::ByteBuffer indexBuff;
			
			UOSInt GetFieldIndex(UOSInt colIndex);
		public:
			FileGDBReader(NotNullPtr<IO::StreamData> fd, UInt64 ofst, FileGDBTableInfo *tableInfo, Data::ArrayListNN<Text::String> *columnNames, UOSInt dataOfst, UOSInt maxCnt, Data::QueryConditions *conditions);
			virtual ~FileGDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged(); //-1 = error
			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Text::String *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);
			virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item);

			virtual Data::VariItem *GetNewItem(const UTF8Char *name);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);

			void SetIndex(NotNullPtr<IO::StreamData> fd, UOSInt indexCnt);
		};
	}
}
#endif
