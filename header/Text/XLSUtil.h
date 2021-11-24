#ifndef _SM_TEXT_XLSUTIL
#define _SM_TEXT_XLSUTIL
#include "Data/DateTime.h"

namespace Text
{
	class XLSUtil
	{
	public:
		static Double Date2Number(Data::DateTime *dt);
		static void Number2Date(Data::DateTime *dt, Double v);
	};
}
#endif
