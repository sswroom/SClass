#ifndef _SM_IO_STREAMREADER
#define _SM_IO_STREAMREADER
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class StreamReader : public IO::Reader
	{
	private:
		Text::Encoding enc;
		NN<IO::Stream> stm;

		UnsafeArray<UTF8Char> cbuff;
		UIntOS cSize;
		UIntOS cPos;
		UnsafeArray<UInt8> buff;
		UIntOS buffSize;
		Text::LineBreakType lineBreak;

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		StreamReader(NN<IO::Stream> stm);
		StreamReader(NN<IO::Stream> stm, UInt32 codePage);
		virtual ~StreamReader();
		virtual void Close();
		virtual UnsafeArrayOpt<UTF8Char> ReadLine(UnsafeArray<UTF8Char> buff, UIntOS maxCharCnt);
		virtual Bool ReadLine(NN<Text::StringBuilderUTF8> sb, UIntOS maxCharCnt);
		virtual UnsafeArray<UTF8Char> GetLastLineBreak(UnsafeArray<UTF8Char> buff);
		virtual Bool GetLastLineBreak(NN<Text::StringBuilderUTF8> sb);
		virtual Bool IsLineBreak();
		virtual Bool ReadToEnd(NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
