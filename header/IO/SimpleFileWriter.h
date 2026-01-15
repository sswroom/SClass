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

		UIntOS WriteBuff(UnsafeArray<const UInt8> buff, UIntOS size);
	public:
		SimpleFileWriter(UnsafeArray<const UTF8Char> fileName, IO::FileMode mode, IO::FileShare share);
		virtual ~SimpleFileWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteW(UnsafeArray<const WChar> str, UIntOS nChar);
		virtual Bool WriteW(UnsafeArray<const WChar> str);
		virtual Bool WriteLineW(UnsafeArray<const WChar> str, UIntOS nChar);
		virtual Bool WriteLineW(UnsafeArray<const WChar> str);
		virtual Bool WriteLine();
	};
}
#endif
