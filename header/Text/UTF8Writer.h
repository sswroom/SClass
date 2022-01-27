#ifndef _SM_TEXT_UTF8WRITER
#define _SM_TEXT_UTF8WRITER
#include "IO/Stream.h"
#include "IO/Writer.h"

namespace Text
{
	class UTF8Writer : public IO::Writer
	{
	private:
		IO::Stream *stm;
	public:
		UTF8Writer(IO::Stream *stm);
		virtual ~UTF8Writer();

		virtual Bool WriteStrC(const UTF8Char *str, UOSInt nChar);
		virtual Bool WriteStr(const UTF8Char *str);
		virtual Bool WriteLineC(const UTF8Char *str, UOSInt nChar);
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

		Bool WriteStr(Text::CString str)
		{
			return WriteStrC(str.v, str.leng);
		}

		Bool WriteLine(Text::CString str)
		{
			return WriteLineC(str.v, str.leng);
		}
	};
}
#endif
