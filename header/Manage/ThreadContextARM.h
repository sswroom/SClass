#ifndef _SM_MANAGE_THREADCONTEXTARM
#define _SM_MANAGE_THREADCONTEXTARM
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextARM : public ThreadContext
	{
	private:
		void *context;
		UIntOS procId;
		UIntOS threadId;
	public:
		ThreadContextARM(UIntOS procId, UIntOS threadId, void *context);
		virtual ~ThreadContextARM();

		virtual UIntOS GetRegisterCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetRegister(UIntOS index, UnsafeArray<UTF8Char> buff, UnsafeArray<UInt8> regVal, OutParam<UInt32> regBitCount) const;
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
