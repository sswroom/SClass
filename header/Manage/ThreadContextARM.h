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

		virtual UOSInt GetRegisterCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UOSInt index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual ContextType GetType() const;
		virtual UOSInt GetThreadId() const;
		virtual UOSInt GetProcessId() const;
		virtual UOSInt GetInstAddr() const;
		virtual UOSInt GetStackAddr() const;
		virtual UOSInt GetFrameAddr() const;
		virtual void SetInstAddr(UOSInt instAddr);
		virtual void SetStackAddr(UOSInt stackAddr);
		virtual void SetFrameAddr(UOSInt frameAddr);
		virtual NN<ThreadContext> Clone() const;
		virtual Bool GetRegs(NN<Manage::Dasm::Dasm_Regs> regs) const;
		virtual Optional<Manage::Dasm> CreateDasm() const;
		UInt32 GetR0() const;
		UInt32 GetR1() const;
		UInt32 GetR2() const;
		UInt32 GetR3() const;
		UInt32 GetR4() const;
		UInt32 GetR5() const;
		UInt32 GetR6() const;
		UInt32 GetR7() const;
		UInt32 GetR8() const;
		UInt32 GetR9() const;
		UInt32 GetR10() const;
		UInt32 GetFP() const;
		UInt32 GetIP() const;
		UInt32 GetSP() const;
		UInt32 GetPC() const;
		UInt32 GetLR() const;
		UInt32 GetCPSR() const;
		void *GetContext() const;
	};
}
#endif
