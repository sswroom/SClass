#ifndef _SM_TEXT_UTF8READER
#define _SM_TEXT_UTF8READER
#include "IO/Stream.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class UTF8Reader
	{
	private:
		IO::Stream *stm;

		UInt8 *buff;
		OSInt buffSize;
		OSInt currOfst;
		Int32 lineBreak;

		Int64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		UTF8Reader(IO::Stream *stm);
		~UTF8Reader();
		void Close();
		UTF32Char Peek();
		UTF32Char Read();
		Bool ReadLine(Text::StringBuilderUTF *sb, OSInt maxCharCnt);
		UTF8Char *ReadLine(UTF8Char *u8buff, OSInt maxCharCnt);
		WChar *GetLastLineBreak(WChar *buff);
		Bool GetLastLineBreak(Text::StringBuilderUTF *sb);
		Bool ReadToEnd(Text::StringBuilderUTF *sb);
	};
};
#endif
