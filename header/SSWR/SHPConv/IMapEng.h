#ifndef _SM_SSWR_SHPCONV_IMAPENG
#define _SM_SSWR_SHPCONV_IMAPENG
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace SSWR
{
	namespace SHPConv
	{
		class IMapEng
		{
		public:
			virtual void ParseLabelStr(Text::CString labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2) = 0;
			virtual void FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2) = 0;
			virtual Text::String *GetNewDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, UOSInt currRec, Data::ArrayList<UInt32> *dbcols2) = 0;
		};
	}
}
#endif
