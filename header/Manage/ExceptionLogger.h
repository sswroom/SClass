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
		static void WriteContext(NotNullPtr<IO::Writer> writer, NotNullPtr<IO::Stream> stm, NotNullPtr<Manage::ThreadContext> context, NotNullPtr<Manage::AddressResolver> addrResol);
		static void WriteStackTrace(NotNullPtr<IO::Writer> writer, NotNullPtr<Manage::StackTracer> tracer, NotNullPtr<Manage::AddressResolver> addrResol);
	public:
		static Bool LogToFile(NotNullPtr<Text::String> fileName, UInt32 exCode, Text::CString exName, UOSInt exAddr, NotNullPtr<Manage::ThreadContext> context);
	};
}

#endif
