#ifndef _SM_IO_STREAMWRITER
#define _SM_IO_STREAMWRITER
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"

namespace IO
{
	class StreamWriter : public IO::Writer
	{
	private:
		Text::Encoding enc;
		NN<IO::Stream> stm;
		UInt8 *buff;
		UInt32 buffSize;

	public:
		StreamWriter(NN<IO::Stream> stm, Text::Encoding *enc);
		StreamWriter(NN<IO::Stream> stm, UInt32 codePage);
		virtual ~StreamWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteW(const WChar *str, UOSInt nChar);
		virtual Bool WriteW(const WChar *str);
		virtual Bool WriteLineW(const WChar *str, UOSInt nChar);
		virtual Bool WriteLineW(const WChar *str);
		virtual Bool WriteLine();

		void WriteSignature();
		void Close();
	};
}
#endif
