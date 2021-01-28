//require dbghelp.lib
#ifndef _SM_MANAGE_EXCEPTIONRECORDER
#define _SM_MANAGE_EXCEPTIONRECORDER
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "IO/IWriter.h"
#include "IO/Stream.h"
#include "Manage/AddressResolver.h"
#include "Manage/StackTracer.h"
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

		static const UTF8Char *fileName;
		static ExceptionAction exAction;

		static const UTF8Char *GetExceptionCodeName(UInt32 exCode);
		static Int32 __stdcall ExceptionHandler(void *exInfo);
	public:
		ExceptionRecorder(const UTF8Char *fileName, ExceptionAction exAction);
		~ExceptionRecorder();
	};
};

#endif
