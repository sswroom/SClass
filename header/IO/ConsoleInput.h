#ifndef _SM_IO_CONSOLEINPUT
#define _SM_IO_CONSOLEINPUT
#include "IO/ConsoleWriter.h"
#include "Data/DateTime.h"

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
		static InputReturnType InputSelect(IO::ConsoleWriter *console, const UTF8Char **names, OSInt nNames, OSInt *selection);
		static InputReturnType InputDateTime(IO::ConsoleWriter *console, Data::DateTime *output);
		static InputReturnType InputHexBytes(IO::ConsoleWriter *console, UInt8 *buff, OSInt buffSize, OSInt *inputSize);
		static InputReturnType InputString(IO::ConsoleWriter *console, UTF8Char *output, OSInt maxCharCnt, OSInt *inputSize);
	};
};
#endif
