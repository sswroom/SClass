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
			Data::ArrayList<FileGDBTable *> *tables;

			FileGDBTable *GetTable(const UTF8Char *name);

			FileGDBDir(const UTF8Char *sourceName);
		public:
			virtual ~FileGDBDir();

			virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names);
			virtual DB::DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetErrorMsg(Text::StringBuilderUTF *str);
			virtual void Reconnect();

			void AddTable(FileGDBTable *table);

			static FileGDBDir *OpenDir(IO::PackageFile *pkg);
		};
	}
}
#endif
