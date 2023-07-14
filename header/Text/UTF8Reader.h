#ifndef _SM_TEXT_UTF8READER
#define _SM_TEXT_UTF8READER
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Text/StringBuilderUTF8.h"

#define UTF8READER_BUFFSIZE 16384

namespace Text
{
	class UTF8Reader : public IO::Reader
	{
	private:
		NotNullPtr<IO::Stream> stm;

		UInt8 buff[UTF8READER_BUFFSIZE];
		UOSInt buffSize;
		UOSInt currOfst;
		Text::LineBreakType lineBreak;

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		UTF8Reader(NotNullPtr<IO::Stream> stm);
		virtual ~UTF8Reader();
		virtual void Close();
		UTF32Char Peek();
		UTF32Char Read();
		virtual Bool ReadLine(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt maxCharCnt);
		virtual UTF8Char *ReadLine(UTF8Char *sbuff, UOSInt maxCharCnt);
		virtual UTF8Char *GetLastLineBreak(UTF8Char *buff);
		virtual Bool GetLastLineBreak(NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Bool IsLineBreak();
		virtual Bool ReadToEnd(NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
