#ifndef _SM_MAP_ESRI_FILEGDBTABLE
#define _SM_MAP_ESRI_FILEGDBTABLE
#include "DB/DBReader.h"
#include "IO/StreamData.h"
#include "Map/ESRI/FileGDBUtil.h"

namespace Map
{
	namespace ESRI
	{
		class FileGDBTable
		{
		private:
			NN<Text::String> tableName;
			NN<IO::StreamData> gdbtableFD;
			IO::StreamData *gdbtablxFD;
			UOSInt indexCnt;
			UInt64 dataOfst;
			UInt32 maxRowSize;
			Optional<FileGDBTableInfo> tableInfo;
			NN<Math::ArcGISPRJParser> prjParser;
		public:
			FileGDBTable(Text::CString tableName, NN<IO::StreamData> gdbtableFD, IO::StreamData *gdbtablxFD, NN<Math::ArcGISPRJParser> prjParser);
			~FileGDBTable();

			Bool IsError();
			NN<Text::String> GetName() const;
			Optional<DB::DBReader> OpenReader(Data::ArrayListStringNN *columnNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *conditions);	
		};
	}
}
#endif
