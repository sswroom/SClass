#ifndef _SM_IO_STREAMREADER
#define _SM_IO_STREAMREADER
#include "IO/Reader.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	class StreamReader : public IO::Reader
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

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		StreamReader(IO::Stream *stm);
		StreamReader(IO::Stream *stm, UInt32 codePage);
		virtual ~StreamReader();
		virtual void Close();
		Int32 Peek();
		WChar Read();
		WChar *Read(WChar *buff, UOSInt charCnt);
		WChar *ReadLine(WChar *buff);
		virtual UTF8Char *ReadLine(UTF8Char *buff, UOSInt maxCharCnt);
		WChar *ReadLine(WChar *buff, UOSInt maxCharCnt);
		virtual Bool ReadLine(Text::StringBuilderUTF *sb, UOSInt maxCharCnt);
		virtual UTF8Char *GetLastLineBreak(UTF8Char *buff);
		virtual Bool GetLastLineBreak(Text::StringBuilderUTF *sb);
		WChar *GetLastLineBreak(WChar *buff);
		WChar *ReadToEnd(WChar *buff);
		virtual Bool ReadToEnd(Text::StringBuilderUTF *sb);
	};
}
#endif
