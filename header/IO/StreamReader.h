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
		NotNullPtr<IO::Stream> stm;

		UTF8Char *cbuff;
		UOSInt cSize;
		UOSInt cPos;
		UInt8 *buff;
		UOSInt buffSize;
		Text::LineBreakType lineBreak;

		UInt64 lastPos;
	private:
		void FillBuffer();
		void CheckHeader();
	public:
		StreamReader(NotNullPtr<IO::Stream> stm);
		StreamReader(NotNullPtr<IO::Stream> stm, UInt32 codePage);
		virtual ~StreamReader();
		virtual void Close();
		virtual UTF8Char *ReadLine(UTF8Char *buff, UOSInt maxCharCnt);
		virtual Bool ReadLine(Text::StringBuilderUTF8 *sb, UOSInt maxCharCnt);
		virtual UTF8Char *GetLastLineBreak(UTF8Char *buff);
		virtual Bool GetLastLineBreak(Text::StringBuilderUTF8 *sb);
		virtual Bool IsLineBreak();
		virtual Bool ReadToEnd(Text::StringBuilderUTF8 *sb);
	};
}
#endif
