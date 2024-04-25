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
		static void WriteContext(NN<IO::Writer> writer, NN<IO::Stream> stm, NN<Manage::ThreadContext> context, NN<Manage::AddressResolver> addrResol);
		static void WriteStackTrace(NN<IO::Writer> writer, NN<Manage::StackTracer> tracer, NN<Manage::AddressResolver> addrResol);
	public:
		static Bool LogToFile(NN<Text::String> fileName, UInt32 exCode, Text::CString exName, UOSInt exAddr, NN<Manage::ThreadContext> context);
	};
}

#endif
