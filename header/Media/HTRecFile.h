#ifndef _SM_MEDIA_HTRECFILE
#define _SM_MEDIA_HTRECFILE
#include "Data/ByteBuffer.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/StreamData.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Media
{
	class HTRecFile : public DB::ReadingDB
	{
	private:
		class HTRecReader : public DB::DBReader
		{
		private:
			HTRecFile *file;
			Bool setting;
			UIntOS nextRow;
			UIntOS recCount;
		public:
			HTRecReader(HTRecFile *file, Bool setting);
			virtual ~HTRecReader();

			virtual Bool ReadNext();
			virtual UIntOS ColCount();
			virtual IntOS GetRowChanged();

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

			virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
			virtual Bool IsNull(UIntOS colIndex);
			virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
			virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

			static Text::CString GetName(UIntOS colIndex, Bool setting);
			static Bool GetColDefV(UIntOS colIndex, NN<DB::ColDef> colDef, Bool setting);
		};
	private:
		Int64 time1TS;
		UnsafeArrayOpt<const UTF8Char> serialNo;
		UnsafeArrayOpt<const UTF8Char> testName;
		Int32 address;
		UIntOS totalRecords;
		UInt32 recInterval;
		Int32 tempAlarmL;
		Int32 tempAlarmH;
		Int32 rhAlarmL;
		Int32 rhAlarmH;
		Int64 time2TS;
		Int64 time3TS;
		UIntOS recCount;
		Data::ByteBuffer recBuff;
		Int64 adjStTimeTicks;
		UInt32 adjRecInterval;

	public:
		HTRecFile(NN<IO::StreamData> stmData);
		virtual ~HTRecFile();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Data::Timestamp GetDownloadTime();
		Int32 GetAddress();
		Data::Timestamp GetSettingTime();
		UIntOS GetTotalRec();
		UInt32 GetRecInterval();
		Double GetTempAlarmL();
		Double GetTempAlarmH();
		Double GetHumiAlarmL();
		Double GetHumiAlarmH();
		Data::Timestamp GetStartTime();
		UIntOS GetRecCount();
		UnsafeArrayOpt<UTF8Char> GetSerialNo(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetTestName(UnsafeArray<UTF8Char> sbuff);
		Data::Timestamp GetAdjStartTime();
		UInt32 GetAdjRecInterval();
		const UInt8 *GetRecBuff();
	};
}
#endif
