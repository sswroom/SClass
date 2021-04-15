#ifndef _SM_MANAGE_THREADCONTEXTX86_64
#define _SM_MANAGE_THREADCONTEXTX86_64
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextX86_64 : public ThreadContext
	{
	private:
		void *context;
		UOSInt procId;
		UOSInt threadId;
		OSInt mmOfst;
		OSInt xmmOfst;
	public:
		ThreadContextX86_64(UOSInt procId, UOSInt threadId, void *context);
		virtual ~ThreadContextX86_64();

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
		void *GetContext();

		UInt64 GetRAX();
		UInt64 GetRDX();
		UInt64 GetRCX();
		UInt64 GetRBX();
		UInt64 GetRSI();
		UInt64 GetRDI();
		UInt64 GetRSP();
		UInt64 GetRBP();
		UInt64 GetRIP();
		UInt64 GetR8();
		UInt64 GetR9();
		UInt64 GetR10();
		UInt64 GetR11();
		UInt64 GetR12();
		UInt64 GetR13();
		UInt64 GetR14();
		UInt64 GetR15();
		UInt32 GetEFLAGS();
		UInt16 GetCS();
		UInt16 GetSS();
		UInt16 GetDS();
		UInt16 GetES();
		UInt16 GetFS();
		UInt16 GetGS();
		UInt64 GetDR0();
		UInt64 GetDR1();
		UInt64 GetDR2();
		UInt64 GetDR3();
		UInt64 GetDR6();
		UInt64 GetDR7();
	};
}
#endif
