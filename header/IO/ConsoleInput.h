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
		static InputReturnType InputInt32(IO::ConsoleWriter *console, Int32 *output, Bool showOriVal);
		static InputReturnType InputBool(IO::ConsoleWriter *console, Bool *output);
		static InputReturnType InputSelect(IO::ConsoleWriter *console, Text::String **names, UOSInt nNames, UOSInt *selection);
		static InputReturnType InputDateTime(IO::ConsoleWriter *console, Data::DateTime *output);
		static InputReturnType InputHexBytes(IO::ConsoleWriter *console, UInt8 *buff, UOSInt buffSize, UOSInt *inputSize);
		static InputReturnType InputString(IO::ConsoleWriter *console, UnsafeArray<UTF8Char> output, UOSInt maxCharCnt, UOSInt *inputSize);
	};
}
#endif
