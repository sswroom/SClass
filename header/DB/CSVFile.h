#ifndef _SM_DB_CSVFILE
#define _SM_DB_CSVFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/SeekableStream.h"
#include "IO/StreamData.h"
#include "IO/StreamReader.h"
#include "Text/Encoding.h"

namespace DB
{
	class CSVFile : public DB::ReadingDB
	{
	private:
		NN<Text::String> fileName;
		IO::SeekableStream *stm;
		Bool releaseStm;
		UInt32 codePage;
		Bool noHeader;
		Bool nullIfEmpty;

	public:
		CSVFile(NN<Text::String> fileName, UInt32 codePage);
		CSVFile(Text::CStringNN fileName, UInt32 codePage);
		CSVFile(NN<IO::SeekableStream> stm, UInt32 codePage);
		CSVFile(NN<IO::StreamData> fd, UInt32 codePage);
		virtual ~CSVFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		void SetNoHeader(Bool noHeader);
		void SetNullIfEmpty(Bool nullIfEmpty);
	};

	class CSVReader : public DB::DBReader, public Data::ObjectGetter
	{
	private:
		Optional<IO::Stream> stm;
		NN<IO::Reader> rdr;
		UOSInt nCol;
		UOSInt nHdr;
		UTF8Char *row;
		UOSInt rowBuffSize;
		UTF8Char **cols;
		UOSInt *colSize;
		UTF8Char *hdr;
		Text::PString *hdrs;
		Bool noHeader;
		Bool nullIfEmpty;
		Data::QueryConditions *condition;

	public:
		CSVReader(Optional<IO::Stream> stm, NN<IO::Reader> rdr, Bool noHeader, Bool nullIfEmpty, Data::QueryConditions *condition);
		virtual ~CSVReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();
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

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);
	};
}
#endif
