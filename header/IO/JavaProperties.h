#ifndef _SM_IO_JAVAPROPERTIES
#define _SM_IO_JAVAPROPERTIES
#include "IO/ConfigFile.h"
#include "Text/UTF8Reader.h"

namespace IO
{
	class JavaProperties
	{
	public:
		static Optional<IO::ConfigFile> Parse(Text::CStringNN fileName);
		static Optional<IO::ConfigFile> ParseAppProp();
	private:
		static Optional<IO::ConfigFile> ParseReader(NotNullPtr<Text::UTF8Reader> reader);
		static UTF8Char *EscapeStr(UTF8Char *str, UTF8Char *strEnd);
	};
}
#endif
