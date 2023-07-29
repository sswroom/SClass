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

			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual Bool IsNull(UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);

			static Text::CString GetName(UOSInt colIndex, Bool setting);
			static Bool GetColDefV(UOSInt colIndex, DB::ColDef *colDef, Bool setting);
		};
	private:
		Int64 time1TS;
		const UTF8Char *serialNo;
		const UTF8Char *testName;
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
		HTRecFile(NotNullPtr<IO::StreamData> stmData);
		virtual ~HTRecFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
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
		UTF8Char *GetSerialNo(UTF8Char *sbuff);
		UTF8Char *GetTestName(UTF8Char *sbuff);
		Data::Timestamp GetAdjStartTime();
		UInt32 GetAdjRecInterval();
		const UInt8 *GetRecBuff();
	};
}
#endif
