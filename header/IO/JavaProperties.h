#ifndef _SM_IO_JAVAPROPERTIES
#define _SM_IO_JAVAPROPERTIES
#include "IO/ConfigFile.h"
#include "Text/UTF8Reader.h"

namespace IO
{
	class JavaProperties
	{
	public:
		static IO::ConfigFile *Parse(Text::CString fileName);
		static IO::ConfigFile *ParseAppProp();
	private:
		static IO::ConfigFile *ParseReader(Text::UTF8Reader *reader);
		static UTF8Char *EscapeStr(UTF8Char *str, UTF8Char *strEnd);
	};
}
#endif
