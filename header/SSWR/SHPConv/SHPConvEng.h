#ifndef _SM_SSWR_SHPCONV_SHPCONVENG
#define _SM_SSWR_SHPCONV_SHPCONVENG
#include "Data/ArrayListNative.hpp"
#include "Data/ArrayListObj.hpp"
#include "Text/String.h"

namespace SSWR
{
	namespace SHPConv
	{
		class SHPConvEng
		{
		public:
			virtual void ParseLabelStr(Text::CStringNN labelStr, Data::ArrayListObj<const UTF8Char*> *dbCols, Data::ArrayListNative<UInt32> *dbCols2) = 0;
			virtual void FreeLabelStr(Data::ArrayListObj<const UTF8Char*> *dbCols, Data::ArrayListNative<UInt32> *dbCols2) = 0;
			virtual NN<Text::String> GetNewDBFName(DB::DBFFile *dbf, Data::ArrayListObj<const UTF8Char*> *dbCols, UIntOS currRec, NN<Data::ArrayListNative<UInt32>> dbcols2) = 0;
		};
	}
}
#endif
