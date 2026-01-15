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
			UIntOS rowSize;
			Data::ByteBuffer rowData;
			Int32 objectId;
			UnsafeArray<UInt8> fieldNull;
			UnsafeArray<UInt32> fieldOfst;
			Optional<Data::ArrayListNative<UIntOS>> columnIndices;
			UIntOS dataOfst;
			UIntOS maxCnt;
			UInt32 maxRowSize;
			Optional<Data::QueryConditions> conditions;

			UIntOS indexCnt;
			UIntOS indexNext;
			Data::ByteBuffer indexBuff;
			
			UIntOS GetFieldIndex(UIntOS colIndex);
		public:
			FileGDBReader(NN<IO::StreamData> fd, UInt64 ofst, NN<FileGDBTableInfo> tableInfo, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Optional<Data::QueryConditions> conditions, UInt32 maxRowSize);
			virtual ~FileGDBReader();

			virtual Bool ReadNext();
			virtual UIntOS ColCount();
			virtual IntOS GetRowChanged(); //-1 = error
			virtual Int32 GetInt32(UIntOS colIndex);
			virtual Int64 GetInt64(UIntOS colIndex);
			virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
			virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
			virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
			virtual Double GetDblOrNAN(UIntOS colIndex);
			virtual Bool GetBool(UIntOS colIndex);
			virtual UIntOS GetBinarySize(UIntOS colIndex);
			virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
			virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
			virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);
			virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item);

			virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);

			virtual Bool IsNull(UIntOS colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
			virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
			virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);
			virtual UInt64 GetRowFileOfst() const;

			void SetIndex(NN<IO::StreamData> fd, UIntOS indexCnt);
		};
	}
}
#endif
