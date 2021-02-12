#ifndef _SM_IO_NULLWRITER
#define _SM_IO_NULLWRITER
#include "IO/Writer.h"

namespace IO
{
	class NullWriter : public IO::Writer
	{
	public:
		NullWriter();
		virtual ~NullWriter();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteW(const WChar *str, UOSInt nChar);
		virtual Bool WriteW(const WChar *str);
		virtual Bool WriteLineW(const WChar *str, UOSInt nChar);
		virtual Bool WriteLineW(const WChar *str);
		virtual Bool WriteLine();
	};
}
#endif
