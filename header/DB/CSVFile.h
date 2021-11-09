#ifndef _SM_DB_CSVFILE
#define _SM_DB_CSVFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/SeekableStream.h"
#include "IO/StreamReader.h"
#include "Text/Encoding.h"

namespace DB
{
	class CSVFile : public DB::ReadingDB
	{
	private:
		const UTF8Char *fileName;
		IO::SeekableStream *stm;
		UInt32 codePage;
		Bool noHeader;

	public:
		CSVFile(const UTF8Char *fileName, UInt32 codePage);
		CSVFile(IO::SeekableStream *stm, UInt32 codePage);
		virtual ~CSVFile();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();
		void SetNoHeader(Bool noHeader);
	};

	class CSVReader : public DB::DBReader
	{
	private:
		IO::Stream *stm;
		IO::StreamReader *rdr;
		UOSInt nCol;
		UOSInt nHdr;
		UTF8Char *row;
		UTF8Char **cols;
		UTF8Char *hdr;
		UTF8Char **hdrs;
		Bool noHeader;

	public:
		CSVReader(IO::Stream *stm, IO::StreamReader *rdr, Bool noHeader);
		virtual ~CSVReader();

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
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual void DelNewStr(const UTF8Char *s);
	};
}
#endif
