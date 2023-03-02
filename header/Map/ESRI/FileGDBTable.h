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
			Text::String *tableName;
			IO::StreamData *gdbtableFD;
			IO::StreamData *gdbtablxFD;
			UOSInt indexCnt;
			UInt64 dataOfst;
			FileGDBTableInfo *tableInfo;
			Math::ArcGISPRJParser *prjParser;
		public:
			FileGDBTable(Text::CString tableName, IO::StreamData *gdbtableFD, IO::StreamData *gdbtablxFD, Math::ArcGISPRJParser *prjParser);
			~FileGDBTable();

			Bool IsError();
			Text::String *GetName();
			DB::DBReader *OpenReader(Data::ArrayList<Text::String*> *columnNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *conditions);	
		};
	}
}
#endif
