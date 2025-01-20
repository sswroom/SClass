#ifndef _SM_SSWR_SHPCONV_SHPCONVENG
#define _SM_SSWR_SHPCONV_SHPCONVENG
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvEng
		{
		public:
			virtual void ParseLabelStr(Text::CStringNN labelStr, Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2) = 0;
			virtual void FreeLabelStr(Data::ArrayList<const UTF8Char*> *dbCols, Data::ArrayList<UInt32> *dbCols2) = 0;
			virtual NN<Text::String> GetNewDBFName(DB::DBFFile *dbf, Data::ArrayList<const UTF8Char*> *dbCols, UOSInt currRec, NN<Data::ArrayList<UInt32>> dbcols2) = 0;
		};
	}
}
#endif
