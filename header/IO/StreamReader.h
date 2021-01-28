#ifndef _SM_IO_STREAMREADER
#define _SM_IO_STREAMREADER
#include "IO/Stream.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class StreamReader
	{
	private:
		Text::Encoding *enc;
		IO::Stream *stm;

		WChar *wcbuff;
		UOSInt wcSize;
		UOSInt wcPos;
		UInt8 *buff;
		UOSInt buffSize;
		UOSInt lineBreak;

		Int64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		StreamReader(IO::Stream *stm);
		StreamReader(IO::Stream *stm, Int32 codePage);
		~StreamReader();
		void Close();
		Int32 Peek();
		WChar Read();
		WChar *Read(WChar *buff, UOSInt charCnt);
		WChar *ReadLine(WChar *buff);
		UTF8Char *ReadLine(UTF8Char *buff, UOSInt maxCharCnt);
		WChar *ReadLine(WChar *buff, UOSInt maxCharCnt);
		Bool ReadLine(Text::StringBuilderUTF *sb, UOSInt maxCharCnt);
		UTF8Char *GetLastLineBreak(UTF8Char *buff);
		WChar *GetLastLineBreak(WChar *buff);
		WChar *ReadToEnd(WChar *buff);
		Bool ReadToEnd(Text::StringBuilderUTF *sb);
	};
}
#endif
