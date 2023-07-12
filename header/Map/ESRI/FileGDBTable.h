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
			NotNullPtr<Text::String> tableName;
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
			NotNullPtr<Text::String> GetName() const;
			DB::DBReader *OpenReader(Data::ArrayListNN<Text::String> *columnNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *conditions);	
		};
	}
}
#endif
