#ifndef _SM_TEXT_UTF8WRITER
#define _SM_TEXT_UTF8WRITER
#include "IO/IWriter.h"
#include "IO/Stream.h"

namespace Text
{
	class UTF8Writer : public IO::IWriter
	{
	private:
		IO::Stream *stm;
	public:
		UTF8Writer(IO::Stream *stm);
		virtual ~UTF8Writer();

		virtual Bool Write(const UTF8Char *str, UOSInt nChar);
		virtual Bool Write(const UTF8Char *str);
		virtual Bool WriteLine(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteLine(const UTF8Char *str);
		virtual Bool WriteW(const UTF16Char *str, UOSInt nChar);
		virtual Bool WriteW(const UTF16Char *str);
		virtual Bool WriteLineW(const UTF16Char *str, UOSInt nChar);
		virtual Bool WriteLineW(const UTF16Char *str);
		virtual Bool WriteW(const UTF32Char *str, UOSInt nChar);
		virtual Bool WriteW(const UTF32Char *str);
		virtual Bool WriteLineW(const UTF32Char *str, UOSInt nChar);
		virtual Bool WriteLineW(const UTF32Char *str);
		virtual Bool WriteLine();

		void WriteSignature();
		Int32 Flush();
		void Close();
	};
}
#endif
