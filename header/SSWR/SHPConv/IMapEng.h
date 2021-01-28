#ifndef _SM_SSWR_SHPCONV_IMAPENG
#define _SM_SSWR_SHPCONV_IMAPENG
#include "Data/ArrayList.h"

namespace SSWR
{
	namespace SHPConv
	{
		class IMapEng
		{
		public:
			virtual void ParseLabelStr(const UTF8Char *labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<Int32> *dbCols2) = 0;
			virtual void FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<Int32> *dbCols2) = 0;
			virtual const UTF8Char *GetDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, OSInt currRec, Data::ArrayList<Int32> *dbcols2) = 0;
		};
	}
}
#endif
