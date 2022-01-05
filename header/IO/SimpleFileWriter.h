#ifndef _SM_IO_SIMPLEFILEWRITER
#define _SM_IO_SIMPLEFILEWRITER
#include "IO/FileMode.h"
#include "IO/Writer.h"

namespace IO
{
	class SimpleFileWriter : public Writer
	{
	private:
		void *handle;

		UOSInt WriteBuff(const UInt8 *buff, UOSInt size);
	public:
		SimpleFileWriter(const UTF8Char *fileName, IO::FileMode mode, IO::FileShare share);
		virtual ~SimpleFileWriter();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteStr(const UTF8Char *str);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteW(const WChar *str, UOSInt nChar);
		virtual Bool WriteW(const WChar *str);
		virtual Bool WriteLineW(const WChar *str, UOSInt nChar);
		virtual Bool WriteLineW(const WChar *str);
		virtual Bool WriteLine();
	};
}
#endif
