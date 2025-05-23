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
			UOSInt nextRow;
			UOSInt recCount;
		public:
			HTRecReader(HTRecFile *file, Bool setting);
			virtual ~HTRecReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged();

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

			virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
			virtual Bool IsNull(UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

			static Text::CString GetName(UOSInt colIndex, Bool setting);
			static Bool GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef, Bool setting);
		};
	private:
		Int64 time1TS;
		UnsafeArrayOpt<const UTF8Char> serialNo;
		UnsafeArrayOpt<const UTF8Char> testName;
		Int32 address;
		UOSInt totalRecords;
		UInt32 recInterval;
		Int32 tempAlarmL;
		Int32 tempAlarmH;
		Int32 rhAlarmL;
		Int32 rhAlarmH;
		Int64 time2TS;
		Int64 time3TS;
		UOSInt recCount;
		Data::ByteBuffer recBuff;
		Int64 adjStTimeTicks;
		UInt32 adjRecInterval;

	public:
		HTRecFile(NN<IO::StreamData> stmData);
		virtual ~HTRecFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Data::Timestamp GetDownloadTime();
		Int32 GetAddress();
		Data::Timestamp GetSettingTime();
		UOSInt GetTotalRec();
		UInt32 GetRecInterval();
		Double GetTempAlarmL();
		Double GetTempAlarmH();
		Double GetHumiAlarmL();
		Double GetHumiAlarmH();
		Data::Timestamp GetStartTime();
		UOSInt GetRecCount();
		UnsafeArrayOpt<UTF8Char> GetSerialNo(UnsafeArray<UTF8Char> sbuff);
		UnsafeArrayOpt<UTF8Char> GetTestName(UnsafeArray<UTF8Char> sbuff);
		Data::Timestamp GetAdjStartTime();
		UInt32 GetAdjRecInterval();
		const UInt8 *GetRecBuff();
	};
}
#endif
