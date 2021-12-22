#ifndef _SM_IO_DEBUGWRITER
#define _SM_IO_DEBUGWRITER
#include "IO/Writer.h"

namespace IO
{
	class DebugWriter : public IO::Writer
	{
	private:
		void *clsData;
	public:
		DebugWriter();
		virtual ~DebugWriter();

		void Flush();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteLine();
	};
}
#endif
