#ifndef _SM_IO_SIMPLEFILEWRITER
#define _SM_IO_SIMPLEFILEWRITER
#include "IO/FileStream.h"
#include "IO/IWriter.h"

namespace IO
{
	class SimpleFileWriter : public IWriter
	{
	private:
		void *handle;

		UOSInt WriteBuff(const UInt8 *buff, UOSInt size);
	public:
		SimpleFileWriter(const UTF8Char *fileName, IO::FileStream::FileMode mode, IO::FileStream::FileShare share);
		virtual ~SimpleFileWriter();

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
