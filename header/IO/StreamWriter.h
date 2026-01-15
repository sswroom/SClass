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
		UnsafeArray<UInt8> buff;
		UInt32 buffSize;

	public:
		StreamWriter(NN<IO::Stream> stm, NN<Text::Encoding> enc);
		StreamWriter(NN<IO::Stream> stm, UInt32 codePage);
		virtual ~StreamWriter();

		virtual Bool Write(Text::CStringNN str);
		virtual Bool WriteLine(Text::CStringNN str);
		virtual Bool WriteW(UnsafeArray<const WChar> str, UIntOS nChar);
		virtual Bool WriteW(UnsafeArray<const WChar> str);
		virtual Bool WriteLineW(UnsafeArray<const WChar> str, UIntOS nChar);
		virtual Bool WriteLineW(UnsafeArray<const WChar> str);
		virtual Bool WriteLine();

		void WriteSignature();
		void Close();
	};
}
#endif
