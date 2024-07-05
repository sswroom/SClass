#ifndef _SM_IO_CONSOLEINPUT
#define _SM_IO_CONSOLEINPUT
#include "Data/DateTime.h"
#include "IO/ConsoleWriter.h"
#include "Text/String.h"

namespace IO
{
	class ConsoleInput
	{
	public:
		typedef enum
		{
			IRT_UNKNOWN,
			IRT_ESCAPE,
			IRT_ENTER,
			IRT_ENTEREMPTY,
			IRT_TAB,
			IRT_TABEMPTY
		} InputReturnType;
	public:
		static InputReturnType InputInt32(NN<IO::ConsoleWriter> console, InOutParam<Int32> output, Bool showOriVal);
		static InputReturnType InputBool(NN<IO::ConsoleWriter> console, InOutParam<Bool> output);
		static InputReturnType InputSelect(NN<IO::ConsoleWriter> console, UnsafeArray<NN<Text::String>> names, UOSInt nNames, InOutParam<UOSInt> selection);
		static InputReturnType InputDateTime(NN<IO::ConsoleWriter> console, NN<Data::DateTime> output);
		static InputReturnType InputHexBytes(NN<IO::ConsoleWriter> console, UnsafeArray<UInt8> buff, UOSInt buffSize, OutParam<UOSInt> inputSize);
		static InputReturnType InputString(NN<IO::ConsoleWriter> console, UnsafeArray<UTF8Char> output, UOSInt maxCharCnt, OutParam<UOSInt> inputSize);
	};
}
#endif
