#ifndef _SM_IO_DEBUGWRITER
#define _SM_IO_DEBUGWRITER
#include "IO/Writer.h"

namespace IO
{
	class DebugWriter : public IO::Writer
	{
	private:
		struct ClassData;
		ClassData *clsData;
	public:
		DebugWriter();
		virtual ~DebugWriter();

		void Flush();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine();
	};
}
#endif
