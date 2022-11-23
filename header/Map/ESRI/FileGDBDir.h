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

			FileGDBTable *GetTable(Text::CString name);

			FileGDBDir(Text::String *sourceName);
		public:
			virtual ~FileGDBDir();

			virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
			virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
			virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
			virtual void Reconnect();

			void AddTable(FileGDBTable *table);

			static FileGDBDir *OpenDir(IO::PackageFile *pkg);
		};
	}
}
#endif
