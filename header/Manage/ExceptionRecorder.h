//require dbghelp.lib
#ifndef _SM_MANAGE_EXCEPTIONRECORDER
#define _SM_MANAGE_EXCEPTIONRECORDER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Manage/AddressResolver.h"
#include "Manage/StackTracer.h"
#include "Text/CString.h"
#include "Text/StringBuilder.h"

namespace Manage
{
	class ExceptionRecorder
	{
	public:
		typedef enum
		{
			EA_CLOSE,
			EA_CONTINUE,
			EA_RESTART
		} ExceptionAction;
	private:

		static NN<Text::String> fileName;
		static ExceptionAction exAction;

		static Text::CString GetExceptionCodeName(UInt32 exCode);
		static Int32 __stdcall ExceptionHandler(void *exInfo);
	public:
		ExceptionRecorder(Text::CStringNN fileName, ExceptionAction exAction);
		~ExceptionRecorder();
	};
}

#endif
