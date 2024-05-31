#ifndef _SM_MAP_ESRI_FILEGDBDIR
#define _SM_MAP_ESRI_FILEGDBDIR
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
			Data::ArrayList<FileGDBTable *> tables;

			FileGDBTable *GetTable(Text::CStringNN name);

			FileGDBDir(NN<Text::String> sourceName);
		public:
			virtual ~FileGDBDir();

			virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
			virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
			virtual void CloseReader(NN<DB::DBReader> r);
			virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
			virtual void Reconnect();

			void AddTable(FileGDBTable *table);

			static FileGDBDir *OpenDir(NN<IO::PackageFile> pkg, NN<Math::ArcGISPRJParser> prjParser);
		};
	}
}
#endif
