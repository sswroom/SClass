#ifndef _SM_MAP_ESRI_FILEGDBDIR
#define _SM_MAP_ESRI_FILEGDBDIR
#include "Data/FastStringMapNative.hpp"
#include "Data/FastStringMapNN.hpp"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "IO/PackageFile.h"
#include "Map/ESRI/FileGDBTable.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBDir : public DB::DBConn
		{
		private:
			NN<IO::PackageFile> pkg;
			Data::FastStringMapNative<Int32> tableMap;
			Data::FastStringMapNN<FileGDBTable> tables;
			Data::ArrayListStringNN tableNames;
			NN<Math::ArcGISPRJParser> prjParser;
			Int8 tzQhr;
			Optional<Text::String> lastErrorMsg;
			DB::SQLType sqlType;

			FileGDBDir(NN<IO::PackageFile> pkg, NN<FileGDBTable> systemCatalog, NN<Math::ArcGISPRJParser> prjParser, DB::SQLType sqlType);
		public:
			virtual ~FileGDBDir();

			virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
			virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
			virtual void CloseReader(NN<DB::DBReader> r);
			virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
			virtual void Reconnect();
			Bool IsError() const;

			virtual DB::SQLType GetSQLType() const;
			virtual ConnType GetConnType() const;
			virtual Int8 GetTzQhr() const;
			virtual void ForceTz(Int8 tzQhr);
			virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
			virtual void Close();
			virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
			virtual Optional<DB::DBReader> ExecuteReader(Text::CStringNN sql);
			virtual Bool IsLastDataError();

			virtual Optional<DB::DBTransaction> BeginTransaction();
			virtual void Commit(NN<DB::DBTransaction> tran);
			virtual void Rollback(NN<DB::DBTransaction> tran);

			Optional<FileGDBTable> GetTable(Text::CStringNN name);

			static Optional<FileGDBDir> OpenDir(NN<IO::PackageFile> pkg);
		};
	}
}
#endif
