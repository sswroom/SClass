#ifndef _SM_MAP_ESRI_FILEGDBTABLE
#define _SM_MAP_ESRI_FILEGDBTABLE
#include "IO/IStreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBTable
		{
		private:
			FileGDBTableInfo *tableInfo;
		public:
			FileGDBTable(const UTF8Char *tableName, IO::IStreamData *fd);
			~FileGDBTable();
		};
	}
}
#endif
