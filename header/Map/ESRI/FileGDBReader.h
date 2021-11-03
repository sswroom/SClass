#ifndef _SM_MAP_ESRI_FILEGDBREADER
#define _SM_MAP_ESRI_FILEGDBREADER
#include "DB/DBReader.h"
#include "IO/IStreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBReader : public DB::DBReader
		{
		private:
			IO::IStreamData *fd;
			UInt64 currOfst;
			FileGDBTableInfo *tableInfo;
			UOSInt rowSize;

		public:
			FileGDBReader(IO::IStreamData *fd, UInt64 ofst, FileGDBTableInfo *tableInfo);
			virtual ~FileGDBReader();

		};
	}
}
#endif
