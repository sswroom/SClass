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

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteStr(const UTF8Char *str);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();
	};
}
#endif
