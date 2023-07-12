#ifndef _SM_MANAGE_EXCEPTIONLOGGER
#define _SM_MANAGE_EXCEPTIONLOGGER
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Manage/AddressResolver.h"
#include "Manage/StackTracer.h"
#include "Text/CString.h"

namespace Manage
{
	class ExceptionLogger
	{
	private:
		static void WriteContext(IO::Writer *writer, IO::Stream *stm, Manage::ThreadContext *context, Manage::AddressResolver *addrResol);
		static void WriteStackTrace(IO::Writer *writer, Manage::StackTracer *tracer, Manage::AddressResolver *addrResol);
	public:
		static Bool LogToFile(Text::String *fileName, UInt32 exCode, Text::CString exName, UOSInt exAddr, Manage::ThreadContext *context);
	};
}

#endif
