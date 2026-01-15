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
			Optional<IO::StreamData> gdbtablxFD;
			UIntOS indexCnt;
			UInt64 dataOfst;
			UInt32 maxRowSize;
			Optional<FileGDBTableInfo> tableInfo;
			NN<Math::ArcGISPRJParser> prjParser;
		public:
			FileGDBTable(Text::CStringNN tableName, NN<IO::StreamData> gdbtableFD, Optional<IO::StreamData> gdbtablxFD, NN<Math::ArcGISPRJParser> prjParser);
			~FileGDBTable();

			Bool IsError();
			NN<Text::String> GetName() const;
			NN<Text::String> GetFileName() const;
			Optional<DB::DBReader> OpenReader(Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> conditions);	
		};
	}
}
#endif
