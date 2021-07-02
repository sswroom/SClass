#ifndef _SM_MANAGE_THREADCONTEXTARM
#define _SM_MANAGE_THREADCONTEXTARM
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextARM : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
	public:
		ThreadContextARM(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextARM();

		virtual UOSInt GetRegisterCnt();
		virtual UTF8Char *GetRegister(UOSInt index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount);
		virtual void ToString(Text::StringBuilderUTF *sb);
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
		UInt32 GetR0();
		UInt32 GetR1();
		UInt32 GetR2();
		UInt32 GetR3();
		UInt32 GetR4();
		UInt32 GetR5();
		UInt32 GetR6();
		UInt32 GetR7();
		UInt32 GetR8();
		UInt32 GetR9();
		UInt32 GetR10();
		UInt32 GetFP();
		UInt32 GetIP();
		UInt32 GetSP();
		UInt32 GetPC();
		UInt32 GetLR();
		UInt32 GetCPSR();
		void *GetContext();
	};
}
#endif
