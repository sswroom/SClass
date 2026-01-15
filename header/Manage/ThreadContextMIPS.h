#ifndef _SM_MANAGE_THREADCONTEXTMIPS
#define _SM_MANAGE_THREADCONTEXTMIPS
#include "Manage/ThreadContext.h"

namespace Manage
{
	class ThreadContextMIPS : public ThreadContext
	{
	private:
		void *context;
		UIntOS procId;
		UIntOS threadId;
	public:
		ThreadContextMIPS(UIntOS procId, UIntOS threadId, void *context);
		virtual ~ThreadContextMIPS();

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
		void *GetContext() const;
	};
};
#endif
