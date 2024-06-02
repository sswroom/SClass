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

		UOSInt NewProcess(Text::CStringNN cmdLine);
	public:
		ProcessExecution(Text::CStringNN cmdLine);
		virtual ~ProcessExecution();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(UnsafeArray<const UInt8> buff, UOSInt size);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
