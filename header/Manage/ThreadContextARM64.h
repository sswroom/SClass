#ifndef _SM_MANAGE_THREADCONTEXTARM64
#define _SM_MANAGE_THREADCONTEXTARM64
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextARM64 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
	public:
		ThreadContextARM64(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextARM64();

		virtual UOSInt GetRegisterCnt();
		virtual UTF8Char *GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount);
		virtual void ToString(Text::StringBuilderUTF8 *sb);
		virtual ContextType GetType();
		virtual UOSInt GetThreadId();
		virtual UOSInt GetProcessId();
		virtual UOSInt GetInstAddr();
		virtual UOSInt GetStackAddr();
		virtual UOSInt GetFrameAddr();
		virtual void SetInstAddr(UOSInt instAddr);
		virtual void SetStackAddr(UOSInt stackAddr);
		virtual void SetFrameAddr(UOSInt frameAddr);
		virtual ThreadContext *Clone();
		virtual Bool GetRegs(Manage::Dasm::Dasm_Regs *regs);
		virtual Manage::Dasm *CreateDasm();
		void *GetContext();
	};
}
#endif
