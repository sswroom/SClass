#ifndef _SM_MAP_ESRI_FILEGDBDIR
#define _SM_MAP_ESRI_FILEGDBDIR
#include "Data/FastStringMap.h"
#include "Data/FastStringMapNN.h"
#include "DB/ReadingDB.h"
#include "IO/PackageFile.h"
#include "Map/ESRI/FileGDBTable.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBDir : public DB::ReadingDB
		{
		private:
			NN<IO::PackageFile> pkg;
			Data::FastStringMap<Int32> tableMap;
			Data::FastStringMapNN<FileGDBTable> tables;
			Data::ArrayListStringNN tableNames;
			NN<Math::ArcGISPRJParser> prjParser;

			FileGDBDir(NN<IO::PackageFile> pkg, NN<FileGDBTable> systemCatalog, NN<Math::ArcGISPRJParser> prjParser);
		public:
			virtual ~FileGDBDir();

			virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
			virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
			virtual void CloseReader(NN<DB::DBReader> r);
			virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
			virtual void Reconnect();
			Bool IsError() const;

			Optional<FileGDBTable> GetTable(Text::CStringNN name);

			static Optional<FileGDBDir> OpenDir(NN<IO::PackageFile> pkg);
		};
	}
}
#endif
