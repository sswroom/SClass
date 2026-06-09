#ifndef _SM_IO_LOGZIPPER
#define _SM_IO_LOGZIPPER
#include "IO/ZIPMTBuilder.h"
#include "Text/CString.h"

namespace IO
{
	class LogZipper
	{
	private:
		static Bool ZipDir(NN<IO::ZIPMTBuilder> zip, UnsafeArray<UTF8Char> filePath, UnsafeArray<UTF8Char> filePathEnd, UnsafeArray<UTF8Char> refPathStart);
	public:
		static Bool ZipLogs(Text::CStringNN logDir);
	};
}
#endif
