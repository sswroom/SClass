#ifndef _SM_MANAGE_THREADCONTEXTAVR
#define _SM_MANAGE_THREADCONTEXTAVR
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextAVR : public ThreadContext
	{
	private:
		void *context;
		UIntOS procId;
		UIntOS threadId;
	public:
		ThreadContextAVR(UIntOS procId, UIntOS threadId, void *context);
		virtual ~ThreadContextAVR();

		virtual UIntOS GetRegisterCnt() const;
		virtual UTF8Char *GetRegister(UIntOS index, UTF8Char *buff, UInt8 *regVal, UInt32 *regBitCount) const;
		virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
		virtual ContextType GetType() const;
		virtual UIntOS GetThreadId() const;
		virtual UIntOS GetProcessId() const;
		virtual UIntOS GetInstAddr() const;
		virtual UIntOS GetStackAddr() const;
		virtual UIntOS GetFrameAddr() const;
		virtual void SetInstAddr(UIntOS instAddr);
		virtual void SetStackAddr(UIntOS stackAddr);
		virtual void SetFrameAddr(UIntOS frameAddr);
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
