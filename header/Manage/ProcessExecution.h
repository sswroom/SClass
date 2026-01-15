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

		UIntOS NewProcess(Text::CStringNN cmdLine);
	public:
		ProcessExecution(Text::CStringNN cmdLine);
		virtual ~ProcessExecution();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);
		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
