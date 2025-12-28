#ifndef _SM_TEXT_STRINGUTIL
#define _SM_TEXT_STRINGUTIL
#include "Data/DateTime.h"

namespace Text
{
	class StringUtil
	{
	private:
		WChar wbuff[64];
	public:
		StringUtil();
		~StringUtil();

		UnsafeArray<const WChar> ToString(NN<Data::DateTime> dt, UnsafeArray<const Char> pattern);
		UnsafeArray<const WChar> ToString(Double val);
		UnsafeArray<const WChar> ToString(Double val, UnsafeArray<const Char> pattern);
		UnsafeArray<const WChar> ToString(Int32 val);
		UnsafeArray<const WChar> ToString(Int64 val);
	};
}
#endif
