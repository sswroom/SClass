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
		UIntOS indexCol;
		Data::ArrayListNative<UIntOS> timeCols;

		void InitReader(NN<CSVReader> r);
	public:
		CSVFile(NN<Text::String> fileName, UInt32 codePage);
		CSVFile(Text::CStringNN fileName, UInt32 codePage);
		CSVFile(NN<IO::SeekableStream> stm, UInt32 codePage);
		CSVFile(NN<IO::StreamData> fd, UInt32 codePage);
		virtual ~CSVFile();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		void SetNoHeader(Bool noHeader);
		void SetNullIfEmpty(Bool nullIfEmpty);
		void SetIndexCol(UIntOS indexCol);
		void SetTimeCols(Data::DataArray<UIntOS> timeCols);

		static Optional<Data::TableData> LoadAsTableData(Text::CStringNN fileName, UInt32 codePage, UIntOS indexCol, Data::DataArray<UIntOS> timeCols);
	};

	class CSVReader : public DB::DBReader, public Data::ObjectGetter
	{
	private:
		struct CSVColumn
		{
			UIntOS colSize;
			UnsafeArray<UTF8Char> value;
			DB::DBUtil::ColType colType;
		};
	private:
		Optional<IO::Stream> stm;
		NN<IO::Reader> rdr;
		UIntOS nCol;
		UIntOS nHdr;
		UnsafeArray<UTF8Char> row;
		UIntOS rowBuffSize;
		UnsafeArray<CSVColumn> cols;
		UnsafeArray<UTF8Char> hdr;
		UnsafeArray<Text::PString> hdrs;
		Bool noHeader;
		Bool nullIfEmpty;
		UIntOS indexCol;
		Optional<Data::QueryConditions> condition;

	public:
		CSVReader(Optional<IO::Stream> stm, NN<IO::Reader> rdr, Bool noHeader, Bool nullIfEmpty, Optional<Data::QueryConditions> condition);
		virtual ~CSVReader();

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
		virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);
		void SetIndexCol(UIntOS indexCol);
		void AddTimeCol(UIntOS timeCol);
	};
}
#endif
