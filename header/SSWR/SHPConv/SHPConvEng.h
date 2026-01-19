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
			virtual void ParseLabelStr(Text::CStringNN labelStr, NN<Data::ArrayListArr<const UTF8Char>> dbCols, NN<Data::ArrayListNative<UInt32>> dbCols2) = 0;
			virtual void FreeLabelStr(NN<Data::ArrayListArr<const UTF8Char>> dbCols, NN<Data::ArrayListNative<UInt32>> dbCols2) = 0;
			virtual NN<Text::String> GetNewDBFName(NN<DB::DBFFile> dbf, NN<Data::ArrayListArr<const UTF8Char>> dbCols, UIntOS currRec, NN<Data::ArrayListNative<UInt32>> dbcols2) = 0;
		};
	}
}
#endif
