#ifndef _SM_TEXT_UTF8READER
#define _SM_TEXT_UTF8READER
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class UTF8Reader : public IO::Reader
	{
	private:
		IO::Stream *stm;

		UInt8 *buff;
		UOSInt buffSize;
		UOSInt currOfst;
		Int32 lineBreak;

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		UTF8Reader(IO::Stream *stm);
		virtual ~UTF8Reader();
		virtual void Close();
		UTF32Char Peek();
		UTF32Char Read();
		virtual Bool ReadLine(Text::StringBuilderUTF8 *sb, UOSInt maxCharCnt);
		virtual UTF8Char *ReadLine(UTF8Char *sbuff, UOSInt maxCharCnt);
		virtual UTF8Char *GetLastLineBreak(UTF8Char *buff);
		virtual Bool GetLastLineBreak(Text::StringBuilderUTF8 *sb);
		virtual Bool ReadToEnd(Text::StringBuilderUTF8 *sb);
	};
}
#endif
