#ifndef _SM_MANAGE_PROCESSEXECUTION
#define _SM_MANAGE_PROCESSEXECUTION
#include "IO/Stream.h"
#include "Manage/Process.h"

namespace Manage
{
	class ProcessExecution : public Process, public IO::Stream
	{
	private:
		struct ClassData;
		ClassData *clsData;

		UOSInt NewProcess(Text::CString cmdLine);
	public:
		ProcessExecution(Text::CString cmdLine);
		virtual ~ProcessExecution();

		virtual Bool IsDown();
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
	};
}
#endif
