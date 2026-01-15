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
		NN<IO::Stream> stm;

		UInt8 buff[UTF8READER_BUFFSIZE];
		UIntOS buffSize;
		UIntOS currOfst;
		Text::LineBreakType lineBreak;

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		UTF8Reader(NN<IO::Stream> stm);
		virtual ~UTF8Reader();
		virtual void Close();
		UTF32Char Peek();
		UTF32Char Read();
		virtual Bool ReadLine(NN<Text::StringBuilderUTF8> sb, UIntOS maxCharCnt);
		virtual UnsafeArrayOpt<UTF8Char> ReadLine(UnsafeArray<UTF8Char> sbuff, UIntOS maxCharCnt);
		virtual UnsafeArray<UTF8Char> GetLastLineBreak(UnsafeArray<UTF8Char> buff);
		virtual Bool GetLastLineBreak(NN<Text::StringBuilderUTF8> sb);
		virtual Bool IsLineBreak();
		virtual Bool ReadToEnd(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
