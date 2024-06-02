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
		static Optional<IO::ConfigFile> ParseReader(NN<Text::UTF8Reader> reader);
		static UnsafeArray<UTF8Char> EscapeStr(UnsafeArray<UTF8Char> str, UnsafeArray<UTF8Char> strEnd);
	};
}
#endif
