#ifndef _SM_MEDIA_HTRECFILE
#define _SM_MEDIA_HTRECFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/IStreamData.h"
#include "Text/StringBuilderUTF.h"

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
			virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
			virtual const UTF8Char *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Vector2D *GetVector(UOSInt colIndex);

			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual Bool IsNull(UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
			virtual void DelNewStr(const UTF8Char *s);

			const UTF8Char *GetName(UOSInt colIndex);
		};
	private:
		Data::DateTime *time1;
		const UTF8Char *serialNo;
		const UTF8Char *testName;
		Int32 address;
		UOSInt totalRecords;
		UInt32 recInterval;
		Int32 tempAlarmL;
		Int32 tempAlarmH;
		Int32 rhAlarmL;
		Int32 rhAlarmH;
		Data::DateTime *time2;
		Data::DateTime *time3;
		UOSInt recCount;
		UInt8 *recBuff;
		Data::DateTime *adjStTime;
		UInt32 adjRecInterval;

	public:
		HTRecFile(IO::IStreamData *stmData);
		virtual ~HTRecFile();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
		virtual DB::DBReader *GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();

		Bool GetDownloadTime(Data::DateTime *t);
		Int32 GetAddress();
		Bool GetSettingTime(Data::DateTime *t);
		UOSInt GetTotalRec();
		UInt32 GetRecInterval();
		Double GetTempAlarmL();
		Double GetTempAlarmH();
		Double GetHumiAlarmL();
		Double GetHumiAlarmH();
		Bool GetStartTime(Data::DateTime *t);
		UOSInt GetRecCount();
		UTF8Char *GetSerialNo(UTF8Char *sbuff);
		UTF8Char *GetTestName(UTF8Char *sbuff);
		Bool GetAdjStartTime(Data::DateTime *t);
		UInt32 GetAdjRecInterval();
		const UInt8 *GetRecBuff();
	};
}
#endif
