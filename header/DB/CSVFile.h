#ifndef _SM_DB_CSVFILE
#define _SM_DB_CSVFILE
#include "Data/TableData.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/SeekableStream.h"
#include "IO/StreamData.h"
#include "IO/StreamReader.h"
#include "Text/Encoding.h"

namespace DB
{
	class CSVReader;
	class CSVFile : public DB::ReadingDB
	{
	private:
		NN<Text::String> fileName;
		Optional<IO::SeekableStream> stm;
		Optional<IO::StreamData> fd;
		UInt32 codePage;
		Bool noHeader;
		Bool nullIfEmpty;
		UOSInt indexCol;
		Data::ArrayList<UOSInt> timeCols;

		void InitReader(NN<CSVReader> r);
	public:
		CSVFile(NN<Text::String> fileName, UInt32 codePage);
		CSVFile(Text::CStringNN fileName, UInt32 codePage);
		CSVFile(NN<IO::SeekableStream> stm, UInt32 codePage);
		CSVFile(NN<IO::StreamData> fd, UInt32 codePage);
		virtual ~CSVFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		void SetNoHeader(Bool noHeader);
		void SetNullIfEmpty(Bool nullIfEmpty);
		void SetIndexCol(UOSInt indexCol);
		void SetTimeCols(Data::DataArray<UOSInt> timeCols);

		static Optional<Data::TableData> LoadAsTableData(Text::CStringNN fileName, UInt32 codePage, UOSInt indexCol, Data::DataArray<UOSInt> timeCols);
	};

	class CSVReader : public DB::DBReader, public Data::ObjectGetter
	{
	private:
		struct CSVColumn
		{
			UOSInt colSize;
			UnsafeArray<UTF8Char> value;
			DB::DBUtil::ColType colType;
		};
	private:
		Optional<IO::Stream> stm;
		NN<IO::Reader> rdr;
		UOSInt nCol;
		UOSInt nHdr;
		UnsafeArray<UTF8Char> row;
		UOSInt rowBuffSize;
		UnsafeArray<CSVColumn> cols;
		UnsafeArray<UTF8Char> hdr;
		UnsafeArray<Text::PString> hdrs;
		Bool noHeader;
		Bool nullIfEmpty;
		UOSInt indexCol;
		Optional<Data::QueryConditions> condition;

	public:
		CSVReader(Optional<IO::Stream> stm, NN<IO::Reader> rdr, Bool noHeader, Bool nullIfEmpty, Optional<Data::QueryConditions> condition);
		virtual ~CSVReader();

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
		virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);
		void SetIndexCol(UOSInt indexCol);
		void AddTimeCol(UOSInt timeCol);
	};
}
#endif
