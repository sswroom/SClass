#ifndef _SM_TEXT_UTF8WRITER
#define _SM_TEXT_UTF8WRITER
#include "IO/Stream.h"
#include "IO/Writer.h"

namespace Text
{
	class UTF8Writer : public IO::Writer
	{
	private:
		NN<IO::Stream> stm;
	public:
		UTF8Writer(NN<IO::Stream> stm);
		virtual ~UTF8Writer();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteW(UnsafeArray<const UTF16Char> str, UIntOS nChar);
		virtual Bool WriteW(UnsafeArray<const UTF16Char> str);
		virtual Bool WriteLineW(UnsafeArray<const UTF16Char> str, UIntOS nChar);
		virtual Bool WriteLineW(UnsafeArray<const UTF16Char> str);
		virtual Bool WriteW(UnsafeArray<const UTF32Char> str, UIntOS nChar);
		virtual Bool WriteW(UnsafeArray<const UTF32Char> str);
		virtual Bool WriteLineW(UnsafeArray<const UTF32Char> str, UIntOS nChar);
		virtual Bool WriteLineW(UnsafeArray<const UTF32Char> str);
		virtual Bool WriteLine();

		void WriteSignature();
		Int32 Flush();
		void Close();
	};
}
#endif
