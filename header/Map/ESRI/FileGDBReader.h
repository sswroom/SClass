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
			UInt64 rowOfst;
			NN<FileGDBTableInfo> tableInfo;
			UOSInt rowSize;
			Data::ByteBuffer rowData;
			Int32 objectId;
			UnsafeArray<UInt8> fieldNull;
			UnsafeArray<UInt32> fieldOfst;
			Optional<Data::ArrayListNative<UOSInt>> columnIndices;
			UOSInt dataOfst;
			UOSInt maxCnt;
			UInt32 maxRowSize;
			Optional<Data::QueryConditions> conditions;

			UOSInt indexCnt;
			UOSInt indexNext;
			Data::ByteBuffer indexBuff;
			
			UOSInt GetFieldIndex(UOSInt colIndex);
		public:
			FileGDBReader(NN<IO::StreamData> fd, UInt64 ofst, NN<FileGDBTableInfo> tableInfo, Optional<Data::ArrayListStringNN> columnNames, UOSInt dataOfst, UOSInt maxCnt, Optional<Data::QueryConditions> conditions, UInt32 maxRowSize);
			virtual ~FileGDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged(); //-1 = error
			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff);
			virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
			virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
			virtual Double GetDblOrNAN(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff);
			virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);
			virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

			virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
			virtual UInt64 GetRowFileOfst() const;

			void SetIndex(NN<IO::StreamData> fd, UOSInt indexCnt);
		};
	}
}
#endif
