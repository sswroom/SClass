#ifndef _SM_TEXT_XLSUTIL
#define _SM_TEXT_XLSUTIL
#include "Data/DateTime.h"
#include "Data/Timestamp.h"

namespace Text
{
	class XLSUtil
	{
	public:
		static Double Date2Number(Data::DateTime *dt);
		static void Number2Date(Data::DateTime *dt, Double v);
		static Data::Timestamp Number2Timestamp(Double v);
	};
}
#endif
