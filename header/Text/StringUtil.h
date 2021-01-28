#ifndef _SM_TEXT_STRINGUTIL
#define _SM_TEXT_STRINGUTIL
#include "Data/DateTime.h"

namespace Text
{
	class StringUtil
	{
	private:
		WChar sbuff[64];
	public:
		StringUtil();
		~StringUtil();

		const WChar *ToString(Data::DateTime *dt, const Char *pattern);
		const WChar *ToString(Double val);
		const WChar *ToString(Double val, const Char *pattern);
		const WChar *ToString(Int32 val);
		const WChar *ToString(Int64 val);
	};
};
#endif
