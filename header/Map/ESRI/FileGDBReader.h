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
			NN<IO::StreamData> fd;
			UInt64 currOfst;
			NN<FileGDBTableInfo> tableInfo;
			UOSInt rowSize;
			Data::ByteBuffer rowData;
			Int32 objectId;
			UInt8 *fieldNull;
			UInt32 *fieldOfst;
			Data::ArrayList<UOSInt> *columnIndices;
			UOSInt dataOfst;
			UOSInt maxCnt;
			UInt32 maxRowSize;
			Data::QueryConditions *conditions;

			UOSInt indexCnt;
			UOSInt indexNext;
			Data::ByteBuffer indexBuff;
			
			UOSInt GetFieldIndex(UOSInt colIndex);
		public:
			FileGDBReader(NN<IO::StreamData> fd, UInt64 ofst, NN<FileGDBTableInfo> tableInfo, Data::ArrayListStringNN *columnNames, UOSInt dataOfst, UOSInt maxCnt, Data::QueryConditions *conditions, UInt32 maxRowSize);
			virtual ~FileGDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged(); //-1 = error
			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);
			virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

			virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

			void SetIndex(NN<IO::StreamData> fd, UOSInt indexCnt);
		};
	}
}
#endif
