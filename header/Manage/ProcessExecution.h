#ifndef _SM_MANAGE_PROCESSEXECUTION
#define _SM_MANAGE_PROCESSEXECUTION
#include "IO/Stream.h"
#include "Manage/Process.h"

namespace Manage
{
	class ProcessExecution : public Process, public IO::Stream
	{
	private:
		void *clsData;

		UOSInt NewProcess(const UTF8Char *cmdLine);
	public:
		ProcessExecution(const UTF8Char *cmdLine);
		virtual ~ProcessExecution();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
