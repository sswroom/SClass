#ifndef _SM_MAP_ESRI_FILEGDBTABLE
#define _SM_MAP_ESRI_FILEGDBTABLE
#include "DB/DBReader.h"
#include "IO/IStreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBTable
		{
		private:
			const UTF8Char *tableName;
			IO::IStreamData *fd;
			UInt64 dataOfst;
			FileGDBTableInfo *tableInfo;
		public:
			FileGDBTable(const UTF8Char *tableName, IO::IStreamData *fd);
			~FileGDBTable();

			Bool IsError();
			const UTF8Char *GetName();
			DB::DBReader *OpenReader(Data::ArrayList<const UTF8Char*> *columnNames, UOSInt dataOfst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *conditions);	
		};
	}
}
#endif
