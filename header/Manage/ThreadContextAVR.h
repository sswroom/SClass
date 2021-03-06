#ifndef _SM_MANAGE_THREADCONTEXTAVR
#define _SM_MANAGE_THREADCONTEXTAVR
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextAVR : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
	public:
		ThreadContextAVR(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextAVR();

		virtual OSInt GetRegisterCnt();
		virtual UTF8Char *GetRegister(OSInt index, UTF8Char *buff, UInt8 *regVal, Int32 *regBitCount);
		virtual void ToString(Text::StringBuilderUTF *sb);
		virtual ContextType GetType();
		virtual UOSInt GetThreadId();
		virtual UOSInt GetProcessId();
		virtual OSInt GetInstAddr();
		virtual OSInt GetStackAddr();
		virtual OSInt GetFrameAddr();
		virtual void SetInstAddr(OSInt instAddr);
		virtual void SetStackAddr(OSInt stackAddr);
		virtual void SetFrameAddr(OSInt frameAddr);
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
};
#endif
